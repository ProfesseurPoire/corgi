#pragma once

#include "IndexBuffer.h"
#include "PhysicalDevice.h"
#include "Pipeline.h"
#include "RenderPass.h"
#include "Swapchain.h"
#include "UniformBufferObject.h"
#include "VertexBuffer.h"
#include "VulkanConstants.h"

#include <vulkan/vulkan_core.h>

#include <optional>
#include <vector>
#include <string>

struct SDL_Window;

struct Mesh
{
    IndexBuffer         ib;
    VertexBuffer        vb;
    UniformBufferObject ubo;
};

class VulkanRenderer
{
public:
    VulkanRenderer(SDL_Window* window);

    VkInstance     instance_;
    VkDevice       device_;
    PhysicalDevice physical_device_;
    SDL_Window*    window_;
    VkSurfaceKHR   surface_;
    Swapchain      swapchain_;
    RenderPass     render_pass_;
    DepthBuffer    depth_buffer_;
    VkCommandPool  commandPool;

    std::vector<VkSemaphore> image_available_semaphores_;
    std::vector<VkSemaphore> render_finished_semaphores_;
    std::vector<VkFence>     in_flight_fences_;

    /**
     * \brief Helper function to create a buffer that stores indexes
     * \param indexes 
     * \return 
     */
    IndexBuffer create_index_buffer(std::span<const uint16_t> indexes);

    VertexBuffer create_vertex_buffer(std::span<Vertex> vertices);
    Pipeline     create_pipeline(const UniformBufferObject& ubo);

    /**
     * \brief   Loads an image file into a Vulkan Image
     * \param path 
     * \return 
     */
    Image create_image(const std::string& path);
    

    void create_command_buffers();
    void create_command_pool();
    void create_sync_objects();

    void draw(const std::vector<std::pair<Mesh, Pipeline>>& meshes);

    void recordCommandBuffer(VkCommandBuffer commandBuffer,
                             uint32_t        imageIndex,
                             int             current_frame,
                             Mesh            mesh,
                             Pipeline        pipeline);

    std::vector<UniformBufferObject> uniform_buffer_objects_;

    UniformBufferObject add_uniform_buffer_object(
        void* data, int size, UniformBufferObject::ShaderStage shader_stage, int layout,
         ImageView image_view, VkSampler sampler);

    VkQueue graphicsQueue;
    VkQueue presentQueue;

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

    const std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
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