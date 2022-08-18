#define VK_USE_PLATFORM_WIN32_KHR
#include "Buffer.h"
#include "Framebuffer.h"
#include "Image.h"
#include "IndexBuffer.h"
#include "PhysicalDevice.h"
#include "Pipeline.h"
#include "RenderPass.h"
#include "Texture.h"
#include "UniformBufferObject.h"
#include "Vertex.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_vulkan.h>
#include <Swapchain.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <algorithm>
#include <deque>
#include <fstream>
#include <iostream>
#include <optional>
#include <set>
#include <span>
#include <vector>

// TODO : Inspect this :
// I'm disabling the main SDL macro because i'm stumbling upon a undefined reference
// thing and I don't really understand why since the rest of the SDL seems to be
// working nicely
#ifdef main
#    undef main
#endif /* main */

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif
const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};

SDL_Window*     window {nullptr};
VkInstance      instance;
VkDevice        device;
VkQueue         graphicsQueue;
VkQueue         presentQueue;
VkSurfaceKHR    surface;
Swapchain       swapchain;
RenderPass      renderPass;
VkCommandPool   commandPool;
VkCommandBuffer commandBuffer;
VkSemaphore     imageAvailableSemaphore;
VkSemaphore     renderFinishedSemaphore;
VkFence         inFlightFence;
Pipeline        pipeline;
VkBuffer        vertexBuffer;

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR        capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR>   presentModes;
};

std::vector<Vertex> vertices = {{-0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f},
                                {0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f},
                                {0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f},
                                {-0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f}};

std::optional<unsigned> present_family_index;
std::optional<unsigned> graphic_family_index;

const std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

VkDebugUtilsMessengerEXT debugMessenger;

bool checkDeviceExtensionSupport(VkPhysicalDevice device)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,
                                         availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(),
                                             deviceExtensions.end());

    for(const auto& extension : availableExtensions)
    {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
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

VkResult
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

void setup_debugger()
{
    if(!enableValidationLayers)
        return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);

    if(CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) !=
       VK_SUCCESS)
    {
        throw std::runtime_error("failed to set up debug messenger!");
    }
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

std::vector<VkExtensionProperties> get_supported_extensions()
{
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
    return extensions;
}

void print_supported_layers(const std::vector<VkLayerProperties>& layers)
{
    std::cout << "Supported Layers : \n";
    for(auto layer : layers)
    {
        std::cout << "\t" << layer.layerName << "\n";
    }
    std::cout.flush();
}

bool is_layer_supported(const std::vector<VkLayerProperties>& supported_layers,
                        const std::string&                    layer_name)
{
    return std::ranges::find_if(supported_layers,
                                [layer_name](const VkLayerProperties& layer) {
                                    return layer.layerName == layer_name;
                                }) != supported_layers.end();
}

bool is_extension_supported(
    const std::vector<VkExtensionProperties>& supported_extensions,
    const std::string&                        extension_name)
{
    return std::ranges::find_if(supported_extensions,
                                [extension_name](const VkExtensionProperties& extension) {
                                    return extension.extensionName == extension_name;
                                }) != supported_extensions.end();
}

void print_supported_extensions(const std::vector<VkExtensionProperties>& extensions)
{
    std::cout << "Supported extensions : \n";
    for(auto extension : extensions)
        std::cout << "\t" << extension.extensionName << "\n";
    std::cout.flush();
}

struct QueueFamilies
{
    std::optional<uint32_t> graphics;
};

void create_command_pool()
{
    VkCommandPoolCreateInfo poolInfo {};
    poolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = graphic_family_index.value();

    if(vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create command pool!");
    }
}

std::vector<VkQueueFamilyProperties> get_queue_families(VkPhysicalDevice device)
{
    // Logic to find graphics queue family
    uint32_t count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);

    std::vector<VkQueueFamilyProperties> queue_families(count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &count, queue_families.data());

    unsigned i = 0u;
    for(const auto& queue_family : queue_families)
    {
        if(queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            graphic_family_index = i;

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
        if(presentSupport)
            present_family_index = i;
        i++;
    }
    return queue_families;
}

std::optional<VkQueueFamilyProperties>
get_graphic_queue(const std::vector<VkQueueFamilyProperties>& queue_families)
{
    for(const auto& queue_family : queue_families)
    {
        if(queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            return queue_family;
    }
    return std::nullopt;
}

unsigned
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

void print_queue_family(const std::vector<VkQueueFamilyProperties>& queue_families)
{
    std::cout << "Queue Families \n";

    for(const auto& queue_family : queue_families)
    {
        if(queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            std::cout << "\t VK_QUEUE_GRAPHICS_BIT \n";
        }
        std::cout << "\t\t queueCount : " << queue_family.queueCount << "\n";
    }
    std::cout.flush();
}

/**
 * @brief Simply print the vulkan extension needed by the SDL
 */
void print_sdl_extensions(const std::vector<const char*>& extensions)
{
    std::cout << "Required Extensions : \n";
    for(auto extension : extensions)
        std::cout << "\t" << extension << "\n";
    std::cout.flush();
}

/**
 * @brief   Retrieve the Vulkan Extensions needed by the SDL
 * 
 *          These informations will then be given to the VkInstanceCreateInfo structure
 */
std::vector<const char*> get_sdl_extensions()
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

void check_if_layers_are_supported(const std::vector<VkLayerProperties>& supported_layers,
                                   const std::vector<const char*>&       layers)
{
    for(auto& layer : layers)
    {
        if(!is_layer_supported(supported_layers, layer))
            std::cout << "Warning : " << layer << " is not supported \n";
    }
    std::cout.flush();
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

void create_vulkan_device(VkPhysicalDevice physical_device, unsigned graphic_queue_index)
{
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

    std::set<unsigned> unique_family_index = {graphic_family_index.value(),
                                              present_family_index.value()};

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

    if(vkCreateDevice(physical_device, &createInfo, nullptr, &device) != VK_SUCCESS)
        throw std::runtime_error("failed to create logical device!");

    vkGetDeviceQueue(device, graphic_queue_index, 0, &graphicsQueue);
    vkGetDeviceQueue(device, present_family_index.value(), 0, &presentQueue);
}

void create_surface()
{
    SDL_SysWMinfo info;
    SDL_VERSION(&info.version);
    SDL_GetWindowWMInfo(window, &info);

    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo {};
    surfaceCreateInfo.sType     = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.hwnd      = info.info.win.window;
    surfaceCreateInfo.hinstance = GetModuleHandle(nullptr);

    if(vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface) !=
       VK_SUCCESS)
    {
        throw std::runtime_error("failed to create window surface!");
    }

    if(!SDL_Vulkan_CreateSurface(window, instance, &surface))
    {
        throw std::runtime_error("failed to create window surface!");
    }
}

void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
    VkCommandBufferBeginInfo beginInfo {};
    beginInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags            = 0;          // Optional
    beginInfo.pInheritanceInfo = nullptr;    // Optional

    if(vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo {};
    renderPassInfo.sType       = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass  = renderPass.render_pass;
    renderPassInfo.framebuffer = swapchain.framebuffers[imageIndex].framebuffer();

    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swapchain.create_info.imageExtent;

    VkClearValue clearColor        = {{{0.0f, 1.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues    = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipeline);

    VkViewport viewport {};
    viewport.x        = 0.0f;
    viewport.y        = 0.0f;
    viewport.width    = static_cast<float>(swapchain.create_info.imageExtent.width);
    viewport.height   = static_cast<float>(swapchain.create_info.imageExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor {};
    scissor.offset = {0, 0};
    scissor.extent = swapchain.create_info.imageExtent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    VkBuffer     vertexBuffers[] = {vertexBuffer};
    VkDeviceSize offsets[]       = {0};

    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT16);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            pipeline.pipeline_layout, 0, 1,
                            &UniformBufferObject::descriptorSets[0], 0, nullptr);

    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    if(vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void createCommandBuffer()
{
    VkCommandBufferAllocateInfo allocInfo {};
    allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool        = commandPool;
    allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if(vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

void createSyncObjects()
{
    VkSemaphoreCreateInfo semaphoreInfo {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    VkFenceCreateInfo fenceInfo {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    if(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphore) !=
           VK_SUCCESS ||
       vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphore) !=
           VK_SUCCESS ||
       vkCreateFence(device, &fenceInfo, nullptr, &inFlightFence) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create semaphores!");
    }
}

void create_vulkan_instance()
{
    auto required_extensions = get_sdl_extensions();
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
    appInfo.pApplicationName   = "Hello Triangle";
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

    if(vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create vulkan instance!");
    }

    setup_debugger();

    auto physical_devices = PhysicalDevice::get_physical_devices(instance);

    std::cout << "Physical Devices\n";
    for(const auto& physical_device : physical_devices)
    {
        physical_device.print();
    }

    auto physical_device = PhysicalDevice::get_best_physical_device(physical_devices);

    create_surface();

    auto queue_families = get_queue_families(physical_device.vulkan_device());
    print_queue_family(queue_families);

    auto graphic_queue       = get_graphic_queue(queue_families);
    auto graphic_queue_index = get_graphic_queue_index(queue_families);

    if(!graphic_queue)
        throw std::runtime_error("No graphic queue");

    create_vulkan_device(physical_device.vulkan_device(), graphic_queue_index);

    swapchain.initialize(physical_device.vulkan_device(), device, surface, window,
                         graphic_family_index, present_family_index);

    renderPass.initialize(device, swapchain.create_info.imageFormat);
    swapchain.initialize_framebuffers(device, renderPass.render_pass);

    UniformBufferObject::createDescriptorSetLayout(device);

    pipeline.initialize(device, renderPass.render_pass, swapchain);

    create_command_pool();

    Image::create_texture_image(device, physical_device.vulkan_device(), commandPool,
                                graphicsQueue);

    Texture::create_texture_image_view(device);
    Texture::createTextureSampler(device, physical_device.vulkan_device());

    vertexBuffer =
        Vertex::create_vertex_buffer(physical_device.vulkan_device(), device, vertices);

    IndexBuffer::create(graphicsQueue, commandPool, device,
                        physical_device.vulkan_device());

    UniformBufferObject::create_uniform_buffers(device, physical_device.vulkan_device());
    UniformBufferObject::create_descriptor_pool(device);
    UniformBufferObject::create_descriptor_sets(device);

    createCommandBuffer();
    createSyncObjects();
}

void init_vulkan()
{
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow("test", 100, 100, 1600, 900,
                              SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN);

    create_vulkan_instance();
}

void draw_frame()
{
    // Before drawing, we need to wait for the fence to be available
    // Vulkan is designed with multithreading in mind, fences are like mutex
    // Fences are used for CPU operations, whereas Semaphore are used for GPU operations
    vkWaitForFences(device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(device, 1, &inFlightFence);

    // We ask the swapchain the image we're going to use for our drawing operations
    // The semaphore will get signaled when we actually acquire the image
    // Though apparently the function will wait until it at least knows the index of
    // the image we're going to draw on
    uint32_t imageIndex;
    vkAcquireNextImageKHR(device, swapchain.swapchain, UINT64_MAX,
                          imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

    UniformBufferObject::updateUniformBuffer(device, 0);

    // We clear the command buffer
    vkResetCommandBuffer(commandBuffer, 0);

    // We run our commands
    recordCommandBuffer(commandBuffer, imageIndex);

    // We submit our commands
    VkSubmitInfo submitInfo {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    // Here we specify the semaphore that needs to be signaled for the queue operation
    // to be launched
    VkSemaphore waitSemaphores[] = {imageAvailableSemaphore};

    // Here we define the semaphore we need to wait for
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount     = 1;
    submitInfo.pWaitSemaphores        = waitSemaphores;
    submitInfo.pWaitDstStageMask      = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &commandBuffer;

    // And here we define the semaphore we're going to signal when the operation succeed
    VkSemaphore signalSemaphores[]  = {renderFinishedSemaphore};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores    = signalSemaphores;

    // VkQueueSubmit being asynchronous, we need to use semaphores.
    // The function is asynchronous, but on the GPU side, it will wait for the
    // imageAvailableSemaphore to be signaled by the vkAcquireNextImageKHR function
    // Fence will also be signaled. I guess it's fine since we'll only
    if(vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFence) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    // We tell the queuePresent operation to wait for the signalSemaphore
    VkPresentInfoKHR presentInfo {};
    presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores    = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swapchain.swapchain};
    presentInfo.swapchainCount  = 1;
    presentInfo.pSwapchains     = swapChains;
    presentInfo.pImageIndices   = &imageIndex;
    presentInfo.pResults        = nullptr;    // Optional

    // We give our image to the presentation queue
    vkQueuePresentKHR(presentQueue, &presentInfo);
}

void main_loop()
{
    bool exit = false;

    while(!exit)
    {
        SDL_Event e;
        while(SDL_PollEvent(&e))
        {
            switch(e.type)
            {
                case SDL_QUIT:
                    exit = true;
                    break;
            }
        }
        draw_frame();
        SDL_GL_SwapWindow(window);
    }
    vkDeviceWaitIdle(device);
}

void cleanup()
{
    if(enableValidationLayers)
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);

    vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);
    vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
    vkDestroyFence(device, inFlightFence, nullptr);
    vkDestroyCommandPool(device, commandPool, nullptr);

    for(auto& framebuffer : swapchain.framebuffers)
        vkDestroyFramebuffer(device, framebuffer.framebuffer(), nullptr);

    vkDestroyPipeline(device, pipeline.pipeline, nullptr);
    vkDestroyPipelineLayout(device, pipeline.pipeline_layout, nullptr);
    vkDestroyRenderPass(device, renderPass.render_pass, nullptr);

    for(auto imageView : swapchain.images_view)
        vkDestroyImageView(device, imageView, nullptr);

    swapchain.finalize(device);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyDevice(device, nullptr);
    vkDestroyInstance(instance, nullptr);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char** argv)
{
    init_vulkan();
    main_loop();
    cleanup();
    return 0;
}