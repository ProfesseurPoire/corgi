#include "Swapchain.h"

#include <SDL2/SDL_vulkan.h>

void Swapchain::finalize(VkDevice device)
{
    vkDestroySwapchainKHR(device, swapchain, nullptr);
}

/*!
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

void Swapchain::initialize(VkPhysicalDevice        physical_device,
                           VkDevice                device,
                           VkSurfaceKHR            surface,
                           SDL_Window*             window,
                           std::optional<unsigned> graphic_family_index,
                           std::optional<unsigned> present_family_index)
{
    auto capabilities = swapchain_get_capabilities(surface, physical_device);
    auto formats      = swapchain_get_surface_formats(physical_device, surface);

    // How the swapchains images are consumed by the monitor.
    // * VK_PRESENT_MODE_IMMEDIATE_KHR
    //      Specifies that the presentation engine does not wait for a vertical blanking
    //      period to update the current image
    // * VK_PRESENT_MODE_MAILBOX_KHR
    //      Specifies that the presentation engine waits for the next vertical blanking
    //      period to update the current image. No tearing
    //
    //      Ok so, this part was hard to understand, but basically, the GPU only stores
    //      1 image, and a new request will replace that image.
    //
    //      And at each vertical blanking period, one request is removed from
    //      the queue and processed
    //
    // VK_PRESENT_MODE_FIFO_KHR
    // VK_PRESENT_MODE_FIFO_RELAXED_KHR
    auto present_modes = swapchain_get_present_modes(physical_device, surface);

    // We abort the program if we have no formats or no present mode
    if(formats.empty() || present_modes.empty())
        abort();

    auto surface_format = chooseSwapSurfaceFormat(formats);
    auto present_mode   = chooseSwapPresentMode(present_modes);
    auto extent         = swapchain_choose_swap_extent(this, window, capabilities);

    uint32_t imageCount = capabilities.minImageCount + 1;

    if(capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
    {
        imageCount = capabilities.maxImageCount;
    }

    create_info         = VkSwapchainCreateInfoKHR();
    create_info.sType   = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = surface;

    create_info.minImageCount    = imageCount;
    create_info.imageFormat      = surface_format.format;
    create_info.imageColorSpace  = surface_format.colorSpace;
    create_info.imageExtent      = extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    unsigned queueFamilyIndices[] = {graphic_family_index.value(),
                                     present_family_index.value()};

    if(graphic_family_index != present_family_index)
    {
        create_info.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices   = queueFamilyIndices;
    }
    else
    {
        create_info.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
        create_info.queueFamilyIndexCount = 0;          // Optional
        create_info.pQueueFamilyIndices   = nullptr;    // Optional
    }

    create_info.preTransform   = capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode    = present_mode;
    create_info.clipped        = VK_TRUE;
    create_info.oldSwapchain   = VK_NULL_HANDLE;

    if(vkCreateSwapchainKHR(device, &create_info, nullptr, &swapchain) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);
    images.resize(imageCount);
    vkGetSwapchainImagesKHR(device, swapchain, &imageCount, images.data());

    initialize_image_views(device);
}

void Swapchain::initialize_framebuffers(VkDevice device, VkRenderPass render_pass)
{
    framebuffers.reserve(images_view.size());

    for(auto& image_view : images_view)
    {
        framebuffers.push_back(Framebuffer(create_info.imageExtent.width,
                                           create_info.imageExtent.height, &image_view,
                                           render_pass, device));
    }
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

void Swapchain::initialize_image_views(VkDevice device)
{
    images_view.resize(images.size());

    // I think imageview kinda correspond to a texture, with the mip level etc

    for(size_t i = 0; i < images.size(); i++)
    {
        VkImageViewCreateInfo createInfo {};

        createInfo.sType                       = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image                       = images[i];
        createInfo.viewType                    = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format                      = create_info.imageFormat;
        createInfo.components.r                = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g                = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b                = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a                = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel   = 0;
        createInfo.subresourceRange.levelCount     = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount     = 1;

        if(vkCreateImageView(device, &createInfo, nullptr, &images_view[i]) != VK_SUCCESS)
            throw std::runtime_error("failed to create image views!");
    }
}
