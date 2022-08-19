#pragma once

#include "PhysicalDevice.h"

#include <vulkan/vulkan_core.h>

#include <optional>
#include <vector>

struct SDL_Window;

class VulkanRenderer
{
public:
    VulkanRenderer(SDL_Window* window);

    VkInstance     instance_;
    VkDevice       device_;
    PhysicalDevice physical_device_;
    SDL_Window*    window_;
    VkSurfaceKHR   surface_;

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

    const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};

    // We need the present queue, to send the images to the monitor
    std::optional<unsigned> present_family_index_;

    // We need the graphic queue for rendering operations
    std::optional<unsigned> graphic_family_index_;

    unsigned graphic_queue_index_;

private:
    void create_physical_device();
    void create_device();
    void create_instance();

    /**
     * @brief   Creates Vulkan Queues
     * 
     *          Every operation in vulkan needs to be submitted to a queue.
     *          The most important one being the graphic queue
     */
    void create_queues();

    std::vector<VkQueueFamilyProperties> get_queue_families();

    void setup_debugger();

    /**
     * @brief Creates the surface object we're going to draw in
     */
    void create_surface();
};