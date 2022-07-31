#include "Swapchain.h"

#include <SDL2/SDL_vulkan.h>

void Swapchain::finalize(VkDevice device)
{
    vkDestroySwapchainKHR(device, swapchain, nullptr);
}

/**
 * @brief Define how the color is coded on the swapchain
 * 
 * @param availableFormats 
 * @return VkSurfaceFormatKHR 
 */
VkSurfaceFormatKHR
chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    // Here we just arbitrarly chose something that kinda works wells for most usage
    for(const auto& availableFormat : availableFormats)
    {
        if(availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
           availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return availableFormat;
        }
    }
    // We just pick a defaut format
    return availableFormats[0];
}

VkPresentModeKHR
chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    for(const auto& availablePresentMode : availablePresentModes)
    {
        if(availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            return availablePresentMode;
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D swapchain_choose_swap_extent(Swapchain*                      swapchain,
                                        SDL_Window*                     window,
                                        const VkSurfaceCapabilitiesKHR& capabilities)
{
    if(capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return capabilities.currentExtent;
    }
    else
    {
        int width, height;

        // Gets the window's dimensions
        SDL_Vulkan_GetDrawableSize(window, &width, &height);

        // We use the windows dimension to initialize our extent object

        VkExtent2D actualExtent = {static_cast<uint32_t>(width),
                                   static_cast<uint32_t>(height)};

        actualExtent.width =
            std::clamp(actualExtent.width, capabilities.minImageExtent.width,
                       capabilities.maxImageExtent.width);
        actualExtent.height =
            std::clamp(actualExtent.height, capabilities.minImageExtent.height,
                       capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

VkSurfaceCapabilitiesKHR swapchain_get_capabilities(VkSurfaceKHR     surface,
                                                    VkPhysicalDevice physical_device)
{
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &capabilities);
    return capabilities;
}

std::vector<VkSurfaceFormatKHR>
swapchain_get_surface_formats(VkPhysicalDevice physical_device, VkSurfaceKHR surface)
{
    std::vector<VkSurfaceFormatKHR> formats;

    // We first call this function to get back how many formats are available
    uint32_t count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &count, nullptr);

    if(count == 0)
        return formats;

    // then we resize our vector with this data, and reinvoke the same function with
    // a pointer to the memory we just allocated
    formats.resize(count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &count,
                                         formats.data());

    return formats;
}

void swapchain_initialize(Swapchain*              swapchain,
                          VkPhysicalDevice        physical_device,
                          VkDevice                device,
                          VkSurfaceKHR            surface,
                          SDL_Window*             window,
                          std::optional<unsigned> graphic_family_index,
                          std::optional<unsigned> present_family_index)
{
    auto capabilities  = swapchain_get_capabilities(surface, physical_device);
    auto formats       = swapchain_get_surface_formats(physical_device, surface);
    auto present_modes = swapchain_get_present_modes(physical_device, surface);

    // We abort the program if we have no formats or no present mode
    if(formats.empty() || present_modes.empty())
        abort();

    swapchain->surface_format = chooseSwapSurfaceFormat(formats);
    swapchain->present_mode   = chooseSwapPresentMode(present_modes);
    swapchain->extent = swapchain_choose_swap_extent(swapchain, window, capabilities);

    uint32_t imageCount = capabilities.minImageCount + 1;

    if(capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
    {
        imageCount = capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo {};
    createInfo.sType   = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;

    createInfo.minImageCount    = imageCount;
    createInfo.imageFormat      = swapchain->surface_format.format;
    createInfo.imageColorSpace  = swapchain->surface_format.colorSpace;
    createInfo.imageExtent      = swapchain->extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    unsigned queueFamilyIndices[] = {graphic_family_index.value(),
                                     present_family_index.value()};

    if(graphic_family_index != present_family_index)
    {
        createInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices   = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;          // Optional
        createInfo.pQueueFamilyIndices   = nullptr;    // Optional
    }

    createInfo.preTransform   = capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode    = swapchain->present_mode;
    createInfo.clipped        = VK_TRUE;
    createInfo.oldSwapchain   = VK_NULL_HANDLE;

    if(vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapchain->swapchain) !=
       VK_SUCCESS)
    {
        throw std::runtime_error("failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(device, swapchain->swapchain, &imageCount, nullptr);
    swapchain->images.resize(imageCount);
    vkGetSwapchainImagesKHR(device, swapchain->swapchain, &imageCount,
                            swapchain->images.data());
}

std::vector<VkPresentModeKHR>
swapchain_get_present_modes(VkPhysicalDevice physical_device, VkSurfaceKHR surface)
{
    uint32_t count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &count, nullptr);

    std::vector<VkPresentModeKHR> present_modes;

    if(count != 0)
    {
        present_modes.resize(count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &count,
                                                  present_modes.data());
    }

    return present_modes;
}