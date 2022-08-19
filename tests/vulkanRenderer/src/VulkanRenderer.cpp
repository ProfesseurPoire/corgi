#define VK_USE_PLATFORM_WIN32_KHR
#include "VulkanRenderer.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.h>

#include <iostream>

static VkDebugUtilsMessengerEXT debugMessenger;

VulkanRenderer::VulkanRenderer(SDL_Window* window)
    : window_(window)
{
    create_instance();
    setup_debugger();
    create_physical_device();
    create_surface();
    create_queues();
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

    // We loop through every qyeye looking for the graphic queue
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

void VulkanRenderer::create_device() {}
void VulkanRenderer::create_physical_device()
{    // We get the available physical device and print their characteristics
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