#define VK_USE_PLATFORM_WIN32_KHR
#include "PhysicalDevice.h"

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

#include "Framebuffer.h"

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

SDL_Window*                window {nullptr};
VkInstance                 instance;
VkDevice                   device;
VkPipeline                 graphicsPipeline;
VkQueue                    graphicsQueue;
VkQueue                    presentQueue;
VkSurfaceKHR               surface;
Swapchain                  swapchain;
std::vector<VkImageView>   swapChainImageViews;
VkRenderPass               renderPass;
VkPipelineLayout           pipelineLayout;
std::vector<Framebuffer>    swapChainFramebuffers;
VkCommandPool              commandPool;
VkCommandBuffer            commandBuffer;
VkSemaphore                imageAvailableSemaphore;
VkSemaphore                renderFinishedSemaphore;
VkFence                    inFlightFence;

static std::vector<char> readFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if(!file.is_open())
    {
        throw std::runtime_error("failed to open file!");
    }

    size_t            fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR        capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR>   presentModes;
};

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

void create_swap_chain() {}

void create_image_views()
{
    swapChainImageViews.resize(swapchain.images.size());
    for(size_t i = 0; i < swapchain.images.size(); i++)
    {
        VkImageViewCreateInfo createInfo {};
        createInfo.sType                       = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image                       = swapchain.images[i];
        createInfo.viewType                    = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format                      = swapchain.create_info.imageFormat;
        createInfo.components.r                = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g                = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b                = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a                = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel   = 0;
        createInfo.subresourceRange.levelCount     = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount     = 1;

        if(vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]) !=
           VK_SUCCESS)
        {
            throw std::runtime_error("failed to create image views!");
        }
    }
}

void create_render_pass()
{
    VkAttachmentDescription colorAttachment {};
    colorAttachment.format  = swapchain.create_info.imageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

    colorAttachment.loadOp  = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    colorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments    = &colorAttachmentRef;

    VkRenderPassCreateInfo renderPassInfo {};
    renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments    = &colorAttachment;
    renderPassInfo.subpassCount    = 1;
    renderPassInfo.pSubpasses      = &subpass;

    VkSubpassDependency dependency {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;

    dependency.srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies   = &dependency;

    if(vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create render pass!");
    }
}

VkShaderModule createShaderModule(const std::vector<char>& code)
{
    VkShaderModuleCreateInfo createInfo {};
    createInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode    = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    if(vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create shader module!");
    }
    return shaderModule;
}

void create_graphic_pipeline()
{
    auto vertShaderCode = readFile("shaders/vert.spv");
    auto fragShaderCode = readFile("shaders/frag.spv");

    VkShaderModule                  vertShaderModule = createShaderModule(vertShaderCode);
    VkShaderModule                  fragShaderModule = createShaderModule(fragShaderCode);
    VkPipelineShaderStageCreateInfo vertShaderStageInfo {};
    vertShaderStageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage  = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName  = "main";
    VkPipelineShaderStageCreateInfo fragShaderStageInfo {};
    fragShaderStageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName  = "main";
    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo,
                                                      fragShaderStageInfo};

    std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT,
                                                 VK_DYNAMIC_STATE_SCISSOR};

    VkPipelineDynamicStateCreateInfo dynamicState {};
    dynamicState.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates    = dynamicStates.data();

    VkPipelineVertexInputStateCreateInfo vertexInputInfo {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount   = 0;
    vertexInputInfo.pVertexBindingDescriptions      = nullptr;    // Optional
    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    vertexInputInfo.pVertexAttributeDescriptions    = nullptr;    // Optional

    VkPipelineInputAssemblyStateCreateInfo inputAssembly {};
    inputAssembly.sType    = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport {};
    viewport.x        = 0.0f;
    viewport.y        = 0.0f;
    viewport.width    = (float)swapchain.create_info.imageExtent.width;
    viewport.height   = (float)swapchain.create_info.imageExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor {};
    scissor.offset = {0, 0};
    scissor.extent = swapchain.create_info.imageExtent;

    VkPipelineViewportStateCreateInfo viewportState {};
    viewportState.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports    = &viewport;
    viewportState.scissorCount  = 1;
    viewportState.pScissors     = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable        = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode             = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth               = 1.0f;
    rasterizer.cullMode                = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace               = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable         = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f;    // Optional
    rasterizer.depthBiasClamp          = 0.0f;    // Optional
    rasterizer.depthBiasSlopeFactor    = 0.0f;    // Optional

    VkPipelineMultisampleStateCreateInfo multisampling {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable   = VK_FALSE;
    multisampling.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading      = 1.0f;        // Optional
    multisampling.pSampleMask           = nullptr;     // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE;    // Optional
    multisampling.alphaToOneEnable      = VK_FALSE;    // Optional

    VkPipelineColorBlendAttachmentState colorBlendAttachment {};
    colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
        VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable         = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;     // Optional
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;    // Optional
    colorBlendAttachment.colorBlendOp        = VK_BLEND_OP_ADD;         // Optional
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;     // Optional
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;    // Optional
    colorBlendAttachment.alphaBlendOp        = VK_BLEND_OP_ADD;         // Optional

    VkPipelineColorBlendStateCreateInfo colorBlending {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable     = VK_FALSE;
    colorBlending.logicOp           = VK_LOGIC_OP_COPY;    // Optional
    colorBlending.attachmentCount   = 1;
    colorBlending.pAttachments      = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;    // Optional
    colorBlending.blendConstants[1] = 0.0f;    // Optional
    colorBlending.blendConstants[2] = 0.0f;    // Optional
    colorBlending.blendConstants[3] = 0.0f;    // Optional

    VkPipelineLayoutCreateInfo pipelineLayoutInfo {};
    pipelineLayoutInfo.sType          = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;                  // Optional
    pipelineLayoutInfo.pSetLayouts    = nullptr;            // Optional
    pipelineLayoutInfo.pushConstantRangeCount = 0;          // Optional
    pipelineLayoutInfo.pPushConstantRanges    = nullptr;    // Optional

    if(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
       VK_SUCCESS)
    {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    VkGraphicsPipelineCreateInfo pipelineInfo {};
    pipelineInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount          = 2;
    pipelineInfo.pStages             = shaderStages;
    pipelineInfo.pVertexInputState   = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState      = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState   = &multisampling;
    pipelineInfo.pDepthStencilState  = nullptr;    // Optional
    pipelineInfo.pColorBlendState    = &colorBlending;
    pipelineInfo.pDynamicState       = &dynamicState;

    pipelineInfo.layout             = pipelineLayout;
    pipelineInfo.renderPass         = renderPass;
    pipelineInfo.subpass            = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;    // Optional
    pipelineInfo.basePipelineIndex  = -1;                // Optional

    if(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr,
                                 &graphicsPipeline) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    vkDestroyShaderModule(device, fragShaderModule, nullptr);
    vkDestroyShaderModule(device, vertShaderModule, nullptr);
}

// Apparently I make a framebuffer for each swapchain I made?
// Or I make a framebuffer for each image in the swapchain?
void create_framebuffers()
{
    swapChainFramebuffers.reserve(swapChainImageViews.size());

    for(size_t i = 0; i < swapChainImageViews.size(); i++)
    {
        swapChainFramebuffers.push_back( Framebuffer(swapchain.create_info.imageExtent.width,
                                swapchain.create_info.imageExtent.height,
                                &swapChainImageViews[i], renderPass, device));
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
    renderPassInfo.renderPass  = renderPass;
    renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex].framebuffer();

    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swapchain.create_info.imageExtent;

    VkClearValue clearColor        = {{{0.0f, 1.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues    = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

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
    vkCmdDraw(commandBuffer, 3, 1, 0, 0);

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
                         graphic_family_index,
                         present_family_index);

    create_image_views();
    create_render_pass();
    create_graphic_pipeline();
    create_framebuffers();
    create_command_pool();
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
    uint32_t imageIndex;
    vkWaitForFences(device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(device, 1, &inFlightFence);
    vkAcquireNextImageKHR(device, swapchain.swapchain, UINT64_MAX,
                          imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
    vkResetCommandBuffer(commandBuffer, 0);
    recordCommandBuffer(commandBuffer, imageIndex);

    VkSubmitInfo submitInfo {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore          waitSemaphores[] = {imageAvailableSemaphore};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount     = 1;
    submitInfo.pWaitSemaphores        = waitSemaphores;
    submitInfo.pWaitDstStageMask      = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &commandBuffer;

    VkSemaphore signalSemaphores[]  = {renderFinishedSemaphore};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores    = signalSemaphores;

    if(vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFence) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores    = signalSemaphores;
    VkSwapchainKHR swapChains[]    = {swapchain.swapchain};
    presentInfo.swapchainCount     = 1;
    presentInfo.pSwapchains        = swapChains;
    presentInfo.pImageIndices      = &imageIndex;
    presentInfo.pResults           = nullptr;    // Optional
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

    for(auto framebuffer : swapChainFramebuffers)
        vkDestroyFramebuffer(device, framebuffer.framebuffer(), nullptr);

    vkDestroyPipeline(device, graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
    vkDestroyRenderPass(device, renderPass, nullptr);
    for(auto imageView : swapChainImageViews)
    {
        vkDestroyImageView(device, imageView, nullptr);
    }
    swapchain.finalize(device);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyDevice(device, nullptr);
    vkDestroyInstance(instance, nullptr);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void functionstuff(std::span<int> s)
{
    for(auto& integer : s)
    {
        std::cout << "stuff" << integer << std::endl;
    }
}

int main(int argc, char** argv)
{
    std::vector<int>   v {1, 2, 3, 4};
    std::array<int, 5> r {1, 2, 3, 4, 5};
    std::deque<int>    de {1, 2, 3, 4, 5};
    std::set<int>      se {1, 2, 3, 4, 5};

    functionstuff(v);
    functionstuff(r);

    init_vulkan();
    main_loop();
    cleanup();
    return 0;
}