#define VK_USE_PLATFORM_WIN32_KHR
#include "VulkanRenderer.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.h>

#include <iostream>
#include <set>

static VkDebugUtilsMessengerEXT debugMessenger;
static uint32_t                 currentFrame = 0;

std::vector<VkCommandBuffer> command_buffers_;


void VulkanRenderer::create_command_buffers()
{
    command_buffers_.resize(VulkanConstants::max_in_flight);

    VkCommandBufferAllocateInfo allocInfo {};
    allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool        = commandPool;
    allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 2;

    if(vkAllocateCommandBuffers(device_, &allocInfo, command_buffers_.data()) !=
       VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

Pipeline VulkanRenderer::create_pipeline(const UniformBufferObject& ubo)
{
    Pipeline pipeline;
    pipeline.initialize(device_, render_pass_.render_pass, swapchain_, ubo);
    return pipeline;
}

IndexBuffer VulkanRenderer::create_index_buffer(std::span<const uint16_t> indexes)
{
    IndexBuffer ib;
    ib.create(graphicsQueue, commandPool, device_, physical_device_.vulkan_device(),
              indexes);
    return ib;
}

void VulkanRenderer::create_command_pool()
{
    VkCommandPoolCreateInfo poolInfo {};
    poolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = graphic_family_index_.value();

    if(vkCreateCommandPool(device_, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
        throw std::runtime_error("failed to create command pool!");
}

void VulkanRenderer::create_sync_objects()
{
    render_finished_semaphores_.resize(VulkanConstants::max_in_flight);
    image_available_semaphores_.resize(VulkanConstants::max_in_flight);

    in_flight_fences_.resize(VulkanConstants::max_in_flight);

    VkSemaphoreCreateInfo semaphoreInfo {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for(size_t i = 0; i < VulkanConstants::max_in_flight; i++)
    {
        if(vkCreateSemaphore(device_, &semaphoreInfo, nullptr,
                             &image_available_semaphores_[i]) != VK_SUCCESS ||
           vkCreateSemaphore(device_, &semaphoreInfo, nullptr,
                             &render_finished_semaphores_[i]) != VK_SUCCESS ||
           vkCreateFence(device_, &fenceInfo, nullptr, &in_flight_fences_[i]) !=
               VK_SUCCESS)
        {
            throw std::runtime_error("failed to create semaphores!");
        }
    }
}

VulkanRenderer::VulkanRenderer(SDL_Window* window)
    : window_(window)
{
    create_instance();
    setup_debugger();
    create_physical_device();
    create_surface();
    create_queues();
    create_device();
    create_command_pool();

    swapchain_.initialize(physical_device_.vulkan_device(), device_, surface_, window_,
                          graphic_family_index_, present_family_index_);

    
    render_pass_.initialize(device_, physical_device_.vulkan_device(),
                            swapchain_.create_info.imageFormat);

    depth_buffer_.initialize(swapchain_, device_, physical_device_.vulkan_device());

    swapchain_.initialize_framebuffers(depth_buffer_.depthImageView, device_,
                                       render_pass_.render_pass);
    
    create_command_buffers();
    create_sync_objects();
}

Image VulkanRenderer::create_image(const std::string& filepath)
{
    return Image::create_texture_image(device_, physical_device_.vulkan_device(), commandPool,
                                graphicsQueue, filepath);
}


UniformBufferObject VulkanRenderer::add_uniform_buffer_object(
    void* data, int size, UniformBufferObject::ShaderStage shader_stage, int layout, ImageView image_view, VkSampler sampler)
{
    UniformBufferObject ubo;
    ubo.add_uniform(device_, physical_device_.vulkan_device(), data,
                                       size, shader_stage, layout, image_view, sampler);
    ubo.create_descriptor_pool(device_);
    ubo.create_descriptor_sets(device_);

    return ubo;
}

void VulkanRenderer::recordCommandBuffer(VkCommandBuffer commandBuffer,
                                         uint32_t        imageIndex,
                                        int currentFrame,
                                         Mesh            mesh,
                                         Pipeline        pipeline)
{
    mesh.ubo.update(device_, currentFrame);

    VkBuffer     vertexBuffers[] = {mesh.vb.buffer};
    VkDeviceSize offsets[]       = {0};

    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, mesh.ib.indexBuffer, 0,
                         VK_INDEX_TYPE_UINT16);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            pipeline.pipeline_layout, 0, 1, &mesh.ubo.descriptorSets[0],
                            0, nullptr);

    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(mesh.ib.size), 1, 0, 0, 0);
}

VertexBuffer VulkanRenderer::create_vertex_buffer(std::span<Vertex> vertices)
{
    return VertexBuffer::create_vertex_buffer(physical_device_.vulkan_device(), device_,
                                              vertices);
}

void VulkanRenderer::draw(const std::vector<std::pair<Mesh, Pipeline>>& meshes)
{
    // Before drawing, we need to wait for the fence to be available
    // Vulkan is designed with multithreading in mind, fences are like mutex
    // Fences are used for CPU operations, whereas Semaphore are used for GPU operations
    vkWaitForFences(device_, 1, &in_flight_fences_[currentFrame], VK_TRUE, UINT64_MAX);

    vkResetFences(device_, 1, &in_flight_fences_[currentFrame]);

    // We ask the swapchain the image we're going to use for our drawing operations
    // The semaphore will get signaled when we actually acquire the image
    // Though apparently the function will wait until it at least knows the index of
    // the image we're going to draw on
    uint32_t imageIndex;
    vkAcquireNextImageKHR(device_, swapchain_.swapchain, UINT64_MAX,
                          image_available_semaphores_[currentFrame], VK_NULL_HANDLE,
                          &imageIndex);

    // We clear the command buffer
    vkResetCommandBuffer(command_buffers_[currentFrame], 0);

    VkCommandBufferBeginInfo beginInfo {};
    beginInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags            = 0;          // Optional
    beginInfo.pInheritanceInfo = nullptr;    // Optional

    if(vkBeginCommandBuffer(command_buffers_[currentFrame], &beginInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo {};
    renderPassInfo.sType       = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass  = render_pass_.render_pass;
    renderPassInfo.framebuffer = swapchain_.framebuffers[imageIndex].framebuffer();

    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swapchain_.create_info.imageExtent;

    VkClearValue clearColor        = {{{0.0f, 1.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues    = &clearColor;

    vkCmdBeginRenderPass(command_buffers_[currentFrame], &renderPassInfo,
                         VK_SUBPASS_CONTENTS_INLINE);

    // We run our commands
    for(const auto& pair : meshes)
    {
        // So this is the pipeline used
        vkCmdBindPipeline(command_buffers_[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pair.second.pipeline);

        VkViewport viewport {};
        viewport.x        = 0.0f;
        viewport.y        = 0.0f;
        viewport.width    = static_cast<float>(swapchain_.create_info.imageExtent.width);
        viewport.height   = static_cast<float>(swapchain_.create_info.imageExtent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(command_buffers_[currentFrame], 0, 1, &viewport);

        VkRect2D scissor {};
        scissor.offset = {0, 0};
        scissor.extent = swapchain_.create_info.imageExtent;
        vkCmdSetScissor(command_buffers_[currentFrame], 0, 1, &scissor);

        recordCommandBuffer(command_buffers_[currentFrame], imageIndex, currentFrame, pair.first,
                            pair.second);


    }
    vkCmdEndRenderPass(command_buffers_[currentFrame]);

    if(vkEndCommandBuffer(command_buffers_[currentFrame]) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to record command buffer!");
    }

    // We submit our commands
    VkSubmitInfo submitInfo {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    // Here we specify the semaphore that needs to be signaled for the queue operation
    // to be launched
    VkSemaphore waitSemaphores[] = {image_available_semaphores_[currentFrame]};

    // Here we define the semaphore we need to wait for
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount     = 1;
    submitInfo.pWaitSemaphores        = waitSemaphores;
    submitInfo.pWaitDstStageMask      = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &command_buffers_[currentFrame];

    // And here we define the semaphore we're going to signal when the operation succeed
    VkSemaphore signalSemaphores[]  = {render_finished_semaphores_[currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores    = signalSemaphores;

    // VkQueueSubmit being asynchronous, we need to use semaphores.
    // The function is asynchronous, but on the GPU side, it will wait for the
    // imageAvailableSemaphore to be signaled by the vkAcquireNextImageKHR function
    // Fence will also be signaled. I guess it's fine since we'll only
    if(vkQueueSubmit(graphicsQueue, 1, &submitInfo, in_flight_fences_[currentFrame]) !=
       VK_SUCCESS)
        throw std::runtime_error("failed to submit draw command buffer!");

    // We tell the queuePresent operation to wait for the signalSemaphore
    VkPresentInfoKHR presentInfo {};
    presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores    = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swapchain_.swapchain};
    presentInfo.swapchainCount  = 1;
    presentInfo.pSwapchains     = swapChains;
    presentInfo.pImageIndices   = &imageIndex;
    presentInfo.pResults        = nullptr;    // Optional

    // We give our image to the presentation queue
    vkQueuePresentKHR(presentQueue, &presentInfo);

    currentFrame = (currentFrame + 1) % VulkanConstants::max_in_flight;
}

struct QueueFamilies
{
    std::optional<uint32_t> graphics;
};

std::vector<VkQueueFamilyProperties> VulkanRenderer::get_queue_families()
{
    // We first use the vulkan function to get how many queue family there is
    uint32_t count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device_.vulkan_device(), &count,
                                             nullptr);

    // We get back the queue_families
    std::vector<VkQueueFamilyProperties> queue_families(count);
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device_.vulkan_device(), &count,
                                             queue_families.data());

    // We loop through every queue looking for the graphic queue
    unsigned i = 0u;
    for(const auto& queue_family : queue_families)
    {
        // We found the graphic queue. We're saving its index
        if(queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            graphic_family_index_ = i;

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physical_device_.vulkan_device(), i,
                                             surface_, &presentSupport);
        if(presentSupport)
            present_family_index_ = i;
        i++;
    }
    return queue_families;
}

static void print_queue_family(const std::vector<VkQueueFamilyProperties>& queue_families)
{
    std::cout << "Queue Families \n";
    for(const auto& queue_family : queue_families)
    {
        if(queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            std::cout << "\t VK_QUEUE_GRAPHICS_BIT \n";
        std::cout << "\t\t queueCount : " << queue_family.queueCount << "\n";
    }
    std::cout.flush();
}

static std::optional<VkQueueFamilyProperties>
get_graphic_queue(const std::vector<VkQueueFamilyProperties>& queue_families)
{
    for(const auto& queue_family : queue_families)
    {
        if(queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            return queue_family;
    }
    return std::nullopt;
}

static unsigned
get_graphic_queue_index(const std::vector<VkQueueFamilyProperties>& queue_families)
{
    unsigned index = 0;
    for(const auto& queue_family : queue_families)
    {
        if(queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            return index;
        index++;
    }
    return 0;
}

void VulkanRenderer::create_queues()
{
    auto queue_families = get_queue_families();

    print_queue_family(queue_families);

    auto graphic_queue   = get_graphic_queue(queue_families);
    graphic_queue_index_ = get_graphic_queue_index(queue_families);

    if(!graphic_queue)
        throw std::runtime_error("No graphic queue");
}

void VulkanRenderer::create_surface()
{
    SDL_SysWMinfo info;
    SDL_VERSION(&info.version);
    SDL_GetWindowWMInfo(window_, &info);

    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo {};
    surfaceCreateInfo.sType     = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.hwnd      = info.info.win.window;
    surfaceCreateInfo.hinstance = GetModuleHandle(nullptr);

    if(vkCreateWin32SurfaceKHR(instance_, &surfaceCreateInfo, nullptr, &surface_) !=
       VK_SUCCESS)
        throw std::runtime_error("failed to create window surface!");

    if(!SDL_Vulkan_CreateSurface(window_, instance_, &surface_))
        throw std::runtime_error("failed to create window surface!");
}

void VulkanRenderer::create_device()
{
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

    std::set<unsigned> unique_family_index = {graphic_family_index_.value(),
                                              present_family_index_.value()};

    for(auto family_index : unique_family_index)
    {
        VkDeviceQueueCreateInfo queueCreateInfo {};
        queueCreateInfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = family_index;
        queueCreateInfo.queueCount       = 1;
        float queuePriority              = 1.0f;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures {};
    // TODO : Check if the device is suitable
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo createInfo {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.pQueueCreateInfos    = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = queueCreateInfos.size();

    // Adding extensions to the Device
    // We need VK_KHR_SWAPCHAIN_EXTENSION_NAME to draw things
    createInfo.enabledExtensionCount   = deviceExtensions.size();
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    createInfo.pEnabledFeatures = &deviceFeatures;

    if(vkCreateDevice(physical_device_.vulkan_device(), &createInfo, nullptr, &device_) !=
       VK_SUCCESS)
        throw std::runtime_error("failed to create logical device!");

    vkGetDeviceQueue(device_, graphic_queue_index_, 0, &graphicsQueue);
    vkGetDeviceQueue(device_, present_family_index_.value(), 0, &presentQueue);
}

void VulkanRenderer::create_physical_device()
{
    // We get the available physical device and print their characteristics
    auto physical_devices = PhysicalDevice::get_physical_devices(instance_);

    for(const auto& physical_device : physical_devices)
        physical_device.print();

    physical_device_ = PhysicalDevice::get_best_physical_device(physical_devices);
}

/**
 * @brief   Retrieve the Vulkan Extensions needed by the SDL
 * 
 *          These informations will then be given to the VkInstanceCreateInfo structure
 */
static std::vector<const char*> get_sdl_extensions(SDL_Window* window)
{
    std::vector<const char*> extensions;
    unsigned int             extension_count {0};
    // We first use the SDL_Vulkan_GetInstanceExtensions to get how many vulkan extensions
    // are needed by SDL
    SDL_Vulkan_GetInstanceExtensions(window, &extension_count, nullptr);

    // Now that we know how many extensions are needed, we can init our
    // char** to be the right size
    extensions.resize(extension_count);

    // And we call SDL_Vulkan_GetInstanceExtensions another time to retrieve the
    // extension name
    if(!SDL_Vulkan_GetInstanceExtensions(window, &extension_count, extensions.data()))
    {
        // Manage error
        std::cout << "Error while getting vulkan extensions" << std::endl;
    }

    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    return extensions;
}

/**
 * @brief Prints the vulkan extension needed by the SDL
 */
static void print_sdl_extensions(const std::vector<const char*>& extensions)
{
    std::cout << "Required Extensions : \n";
    for(auto extension : extensions)
        std::cout << "\t" << extension << "\n";
    std::cout.flush();
}

/**
 * @brief Prints the supported extensions
 * 
 * @param extensions 
 */
static void
print_supported_extensions(const std::vector<VkExtensionProperties>& extensions)
{
    std::cout << "Supported extensions : \n";
    for(auto extension : extensions)
        std::cout << "\t" << extension.extensionName << "\n";
    std::cout.flush();
}

/**
 * @brief Prints the supported layers
 * 
 * @param layers 
 */
static void print_supported_layers(const std::vector<VkLayerProperties>& layers)
{
    std::cout << "Supported Layers : \n";
    for(auto layer : layers)
        std::cout << "\t" << layer.layerName << "\n";
    std::cout.flush();
}

/**
 * @brief Gets the supported validation layers
 * 
 *        Validation layers helps for debugging purposes. They can be used to hook
 *        debug callbacks to vulkan instance to check what went wrong
 * @return std::vector<VkLayerProperties> 
 */
std::vector<VkLayerProperties> get_supported_layers()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
    return availableLayers;
}

static bool
is_extension_supported(const std::vector<VkExtensionProperties>& supported_extensions,
                       const std::string&                        extension_name)
{
    return std::ranges::find_if(supported_extensions,
                                [extension_name](const VkExtensionProperties& extension) {
                                    return extension.extensionName == extension_name;
                                }) != supported_extensions.end();
}

void check_if_extensions_are_supported(
    const std::vector<VkExtensionProperties>& supported_extensions,
    const std::vector<const char*>&           required_extensions)
{
    for(auto& required_extension : required_extensions)
    {
        if(!is_extension_supported(supported_extensions, required_extension))
            std::cout << "Warning : " << required_extension << " is not supported \n";
    }
    std::cout.flush();
}

std::vector<VkExtensionProperties> get_supported_extensions()
{
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
    return extensions;
}

bool is_layer_supported(const std::vector<VkLayerProperties>& supported_layers,
                        const std::string&                    layer_name)
{
    return std::ranges::find_if(supported_layers,
                                [layer_name](const VkLayerProperties& layer) {
                                    return layer.layerName == layer_name;
                                }) != supported_layers.end();
}

static void
check_if_layers_are_supported(const std::vector<VkLayerProperties>& supported_layers,
                              const std::vector<const char*>&       layers)
{
    for(auto& layer : layers)
    {
        if(!is_layer_supported(supported_layers, layer))
            std::cout << "Warning : " << layer << " is not supported \n";
    }
    std::cout.flush();
}

static VKAPI_ATTR VkBool32 VKAPI_CALL
debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
              VkDebugUtilsMessageTypeFlagsEXT             messageType,
              const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
              void*                                       pUserData)
{
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}

/**
 * @brief Needed to have debug information with VkCreateInstance
 * 
 * @param createInfo 
 */
void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
    createInfo                 = {};
    createInfo.sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

void DestroyDebugUtilsMessengerEXT(VkInstance                   instance,
                                   VkDebugUtilsMessengerEXT     debugMessenger,
                                   const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance, "vkDestroyDebugUtilsMessengerEXT");
    if(func != nullptr)
    {
        func(instance, debugMessenger, pAllocator);
    }
}

static VkResult
CreateDebugUtilsMessengerEXT(VkInstance                                instance,
                             const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                             const VkAllocationCallbacks*              pAllocator,
                             VkDebugUtilsMessengerEXT*                 pDebugMessenger)
{
    // we need to find the actual function
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance, "vkCreateDebugUtilsMessengerEXT");
    if(func != nullptr)
    {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void VulkanRenderer::setup_debugger()
{
    if(!enableValidationLayers)
        return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);

    if(CreateDebugUtilsMessengerEXT(instance_, &createInfo, nullptr, &debugMessenger) !=
       VK_SUCCESS)
    {
        throw std::runtime_error("failed to set up debug messenger!");
    }
}

void VulkanRenderer::create_instance()
{
    auto required_extensions = get_sdl_extensions(window_);
    print_sdl_extensions(required_extensions);

    auto supported_extensions = get_supported_extensions();
    print_supported_extensions(supported_extensions);

    auto supported_layers = get_supported_layers();
    print_supported_layers(supported_layers);

    check_if_extensions_are_supported(supported_extensions, required_extensions);
    check_if_layers_are_supported(supported_layers, validationLayers);

    // Optional information about our application
    VkApplicationInfo appInfo {};
    appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName   = "Hello Corgi";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName        = "Corgi";
    appInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion         = VK_API_VERSION_1_0;

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo {};

    // Tells the Vulkan driver which global extensions and validation layers we want to use
    VkInstanceCreateInfo createInfo {};
    createInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo        = &appInfo;
    createInfo.enabledExtensionCount   = required_extensions.size();
    createInfo.ppEnabledExtensionNames = required_extensions.data();

    if(enableValidationLayers)
    {
        createInfo.enabledLayerCount   = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        populateDebugMessengerCreateInfo(debugCreateInfo);
        // We need to give this structure to pNext to activate debug/validation
        // for vkCreateInstance and vkDestroyInstance
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    }
    else
    {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext             = nullptr;
    }

    if(vkCreateInstance(&createInfo, nullptr, &instance_) != VK_SUCCESS)
        throw std::runtime_error("failed to create vulkan instance!");
}
