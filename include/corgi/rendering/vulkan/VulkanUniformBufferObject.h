#pragma once

#include <corgi/math/Matrix.h>
#include <corgi/rendering/UniformBufferObject.h>
#include <corgi/rendering/vulkan/ImageView.h>
#include <vulkan/vulkan.hpp>

namespace corgi
{
class VulkanUniformBufferObject : public UniformBufferObject
{
public:
    VulkanUniformBufferObject(VkDevice         device,
                              VkPhysicalDevice physical_device,
                              ShaderStage      shader_stage);
    ~VulkanUniformBufferObject();

    void set_data(void* data, int size) override;
    void use() override;
    void update(int current_image = 0) override;

    VkDevice         device_;
    VkPhysicalDevice physical_device_;

    void add_uniform(VkDevice         device,
                     VkPhysicalDevice physical_device,
                     void*            data,
                     int              size,
                     ShaderStage      shader_stage,
                     int              layout,
                     ImageView        image_view,
                     VkSampler        sampler);

    ImageView image_view;
    VkSampler sampler;

    // The descriptor thing
    VkDescriptorPool      descriptorPool;
    VkDescriptorSetLayout descriptorSetLayout;
    VkPipelineLayout      pipelineLayout;

    // The descriptors sets
    std::vector<VkDescriptorSet> descriptorSets;

    // The buffers
    VkBuffer       indexBuffer;
    VkDeviceMemory indexBufferMemory;

    std::vector<VkBuffer>       uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;

    void create_uniform_buffers(VkDevice device, VkPhysicalDevice physical_device);

    void create_descriptor_pool(VkDevice device);
    void create_descriptor_sets(VkDevice device);

    void createDescriptorSetLayout(VkDevice device, ShaderStage shader_stage, int layout);

    void destroy_uniform_buffers(VkDevice device);
    void destroy_descriptor_set_layout(VkDevice device);

private:
};
}    // namespace corgi