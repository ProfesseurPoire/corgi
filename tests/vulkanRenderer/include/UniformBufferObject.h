#pragma once

#include <vulkan/vulkan.hpp>

#include <vector>

struct UniformBufferObject
{
    float model[16];
    float view[16];
    float proj[16];

    static VkDescriptorSetLayout descriptorSetLayout;
    static VkPipelineLayout      pipelineLayout;
    static VkBuffer              indexBuffer;
    static VkDeviceMemory        indexBufferMemory;

    static std::vector<VkBuffer>       uniformBuffers;
    static std::vector<VkDeviceMemory> uniformBuffersMemory;

    static inline void destroy_descriptor_set_layout()
    {
        //vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
    }

    static inline void create_uniform_buffers()
    {
        VkDeviceSize bufferSize = sizeof(UniformBufferObject);

        uniformBuffers.resize(2);
        uniformBuffersMemory.resize(2);

        for(size_t i = 0; i < 2; i++)
        {
            // createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            //              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            //                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            //              uniformBuffers[i], uniformBuffersMemory[i]);
        }
    }

    static inline void destroy_uniform_buffers()
    {
        // for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        // {
        //     vkDestroyBuffer(device, uniformBuffers[i], nullptr);
        //     vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
        // }
    }

    static inline void createDescriptorSetLayout()
    {
        VkDescriptorSetLayoutBinding uboLayoutBinding {};
        uboLayoutBinding.binding         = 0;
        uboLayoutBinding.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.descriptorCount = 1;

        uboLayoutBinding.stageFlags         = VK_SHADER_STAGE_VERTEX_BIT;
        uboLayoutBinding.pImmutableSamplers = nullptr;    // Optional

        VkDescriptorSetLayoutCreateInfo layoutInfo {};
        layoutInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = 1;
        layoutInfo.pBindings    = &uboLayoutBinding;

        // if(vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr,
        //                                &descriptorSetLayout) != VK_SUCCESS)
        // {
        //     throw std::runtime_error("failed to create descriptor set layout!");
        // }

        VkPipelineLayoutCreateInfo pipelineLayoutInfo {};
        pipelineLayoutInfo.sType          = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts    = &descriptorSetLayout;
    }
};