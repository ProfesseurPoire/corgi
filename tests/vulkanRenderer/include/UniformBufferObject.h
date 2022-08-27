#pragma once

#include "ImageView.h"

#include <corgi/math/Matrix.h>
#include <vulkan/vulkan.hpp>

#include <chrono>
#include <vector>

struct UniformBufferObject
{
    // Shader stage where the uniform is used
    enum class ShaderStage
    {
        Vertex,
        Fragment
    };

    struct Uniform
    {
        void* data;
        int   size;
        ShaderStage shader_stage;
        int layout; // Layout index of the uniform in the shader
    };

    void add_uniform(VkDevice         device,
                     VkPhysicalDevice physical_device,
                     void*            data,
                     int              size,
                     ShaderStage      shader_stage,
                     int              layout,
                     ImageView image_view,
                     VkSampler sampler);

    ImageView image_view;
    VkSampler sampler;
    std::vector<Uniform> uniforms;

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

    void update(VkDevice device, uint32_t currentImage);
};
