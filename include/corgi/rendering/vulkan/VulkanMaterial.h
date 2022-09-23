#pragma once

#include <corgi/math/Matrix.h>
#include <corgi/rendering/AbstractMaterial.h>
#include <corgi/rendering/UniformBufferObject.h>
#include <corgi/rendering/vulkan/ImageView.h>
#include <corgi/rendering/vulkan/RenderPass.h>
#include <corgi/rendering/vulkan/VulkanSampler.h>
#include <corgi/rendering/vulkan/VulkanSwapchain.h>
#include <corgi/rendering/vulkan/VulkanUniformBufferObject.h>
#include <corgi/resources/Shader.h>
#include <vulkan/vulkan.hpp>

namespace corgi
{

class VulkanPipeline;

class VulkanMaterial : public AbstractMaterial
{
public:
    // Temporary just so I can create the function for now
    VulkanMaterial() = default;

    VulkanMaterial(VkDevice                     device,
                   VkPhysicalDevice             physical_device,
                   AbstractMaterial::Descriptor descriptor,
                   RenderPass                   render_pass,
                   Swapchain swapchain);

    VulkanMaterial(VkDevice device, VkPhysicalDevice physical_device);

    // The sampler list
    std::vector<corgi::VulkanSampler*>       samplers;
    std::vector<corgi::UniformBufferObject*> ubos;

    ~VulkanMaterial();

    Swapchain  swapchain;
    RenderPass render_pass;

    void set_data(void* data, int size);

    int   binding_ = 0;
    void* data_;
    int   size_;

    //void use() override;

    void update() override;

    VkDevice         device_;
    VkPhysicalDevice physical_device_;

    // The descriptor thing
    VkDescriptorPool      descriptorPool;
    VkDescriptorSetLayout descriptorSetLayout;

    // The descriptors sets
    std::vector<VkDescriptorSet> descriptorSets;

    Shader* vertex_shader_ {nullptr};
    Shader* fragment_shader_ {nullptr};

    // The buffers
    VkBuffer       indexBuffer;
    VkDeviceMemory indexBufferMemory;

    /**
     * \brief Actually init the material after the samplers and uniforms have been set
     */
    void init();

    VkPipelineLayout            pipeline_layout;
    VkPipeline                  pipeline;
    std::vector<VkBuffer>       uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;

    void create_uniform_buffers(VkDevice device, VkPhysicalDevice physical_device);

    void create_descriptor_pool(VkDevice device);
    void create_descriptor_sets(VkDevice device);

    void createDescriptorSetLayout(VkDevice device);

    void destroy_uniform_buffers(VkDevice device);
    void destroy_descriptor_set_layout(VkDevice device);

private:
    void init_pipeline();
};
}    // namespace corgi