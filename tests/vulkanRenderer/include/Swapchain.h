#pragma once

#include <Framebuffer.h>
#include <vulkan/vulkan.hpp>

#include <optional>
#include <vector>

struct SDL_Window;

/**
 * @brief   The swapchain stores images the graphic card will draw unto, before they are presented to the monitor.
 *          Multiple images are used to smooth the process. If we buffer some images, it means the 
 *          monitor don't have to wait for drawing operations to be over on one image.
 * 
 */
struct Swapchain
{
    // The actual buffer images
    std::vector<VkImage> images;

    // The image views
    std::vector<VkImageView> images_view;

    // The framebuffers
    std::vector<Framebuffer> framebuffers;

    // The swapchain identifier
    VkSwapchainKHR swapchain;

    /*!
     * @brief   This structure actually contains all the info we used to construct the
     *          current swapchain
     */
    VkSwapchainCreateInfoKHR create_info;

    /*!
     * @brief Actually initialize the swapchain with all the data it needs
     * 
     * @param physical_device 
     * @param device 
     * @param surface 
     * @param window 
     * @param graphic_family_index 
     * @param present_family_index 
     */
    void initialize(VkPhysicalDevice        physical_device,
                    VkDevice                device,
                    VkSurfaceKHR            surface,
                    SDL_Window*             window,
                    std::optional<unsigned> graphic_family_index,
                    std::optional<unsigned> present_family_index);

    void finalize(VkDevice device);

    /**
     * @brief   Initialize the framebuffers
     * 
     *          We got one framebuffer for each image created by the swapchain.
     * 
     *          Framebuffer needs to specify for which render pass they are compatible,
     *          so that's why we pass this argument
     * 
     * @param render_pass 
     */
    void initialize_framebuffers(VkImageView  depthBuffer,
                                 VkDevice     device,
                                 VkRenderPass render_pass);

private:
    void initialize_image_views(VkDevice device);
};

VkSurfaceCapabilitiesKHR swapchain_get_capabilities(VkSurfaceKHR     surface,
                                                    VkPhysicalDevice physical_device);

VkExtent2D swapchain_choose_swap_extent(Swapchain*                      swapchain,
                                        SDL_Window*                     window,
                                        const VkSurfaceCapabilitiesKHR& capabilities);

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
