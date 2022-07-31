#pragma once

#include <vulkan/vulkan.hpp>

#include <optional>
#include <vector>

struct SDL_Window;

/**
 * @brief The swapchain stores images the graphic card will draw unto, before they are presented to the monitor.
 *          Multiple images are used to smooth the process. If we buffer some images, it means the 
 *          monitor don't have to wait for drawing operations to be over on one image.
 * 
 */
struct Swapchain
{
    // The actual buffer images
    std::vector<VkImage> images;

    // The swapchain identifier
    VkSwapchainKHR swapchain;

    // Structure describing a supported swapchain format-color space pair
    // For now we pick VK_FORMAT_B8G8R8A8_SRGB & VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
    VkSurfaceFormatKHR surface_format;

    // Stores the resolution of the images. Will almost always be the same as the window's resolution
    VkExtent2D extent;

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
    VkPresentModeKHR present_mode;

    void finalize(VkDevice device);
};

VkSurfaceCapabilitiesKHR swapchain_get_capabilities(VkSurfaceKHR     surface,
                                                    VkPhysicalDevice physical_device);

VkExtent2D swapchain_choose_swap_extent(Swapchain*                      swapchain,
                                        SDL_Window*                     window,
                                        const VkSurfaceCapabilitiesKHR& capabilities);

/**
 * @brief Actually initialize the swapchain with all the data it needs
 * 
 * @param swapchain 
 * @param physical_device 
 * @param device 
 * @param surface 
 * @param window 
 * @param graphic_family_index 
 * @param present_family_index 
 */
void swapchain_initialize(Swapchain*              swapchain,
                          VkPhysicalDevice        physical_device,
                          VkDevice                device,
                          VkSurfaceKHR            surface,
                          SDL_Window*             window,
                          std::optional<unsigned> graphic_family_index,
                          std::optional<unsigned> present_family_index);

/**
 * @brief Gets the available VkPresentMode from the hardware
 * 
 * Available values can be : 
 * 
 * * VK_PRESENT_MODE_IMMEDIATE_KHR
 *      Specifies that the presentation engine does not wait for a vertical blanking
 *      period to update the current image
 * * VK_PRESENT_MODE_MAILBOX_KHR
 *      Specifies that the presentation engine waits for the next vertical blanking
 *      period to update the current image. No tearing
 * * VK_PRESENT_MODE_FIFO_KHR
 * * VK_PRESENT_MODE_FIFO_RELAXED_KHR
 * @return std::vector<VkPresentModeKHR> 
 */
std::vector<VkPresentModeKHR>
swapchain_get_present_modes(VkPhysicalDevice physical_device, VkSurfaceKHR surface);

std::vector<VkSurfaceFormatKHR>
swapchain_get_surface_formats(VkPhysicalDevice physical_device, VkSurfaceKHR surface);
