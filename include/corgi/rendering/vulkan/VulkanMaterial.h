#pragma once

#include <corgi/rendering/vulkan/VulkanSampler.h>
#include <corgi/rendering/vulkan/VulkanUniformBufferObject.h>

#include <array>
#include <vector>

namespace corgi
{
class VulkanMaterial
{
public:
    std::vector<VulkanUniformBufferObject> uniform_buffer_objects;
    std::vector<VulkanSampler>             samplers;

    // Not exactly sure what's that
    std::vector<VkDescriptorSet> descriptorSets;
    VkDescriptorPool             descriptorPool;
    VkDescriptorSetLayout        descriptorSetLayout;

    // So once the uniform buffers and the sampler have been initialized
    // we init the material
    // For now, we won't be able to update the material after it has been
    // initialized
    void init(VkDevice device)
    {

        std::vector<VkBuffer> uniformBuffers;

        // So we make as many as many descriptor pool size
        // as there are ubo and samplers
        std::vector<VkDescriptorPoolSize> poolSizes {uniform_buffer_objects.size() +
                                                     samplers.size()};

        int i = 0;
        for(int j = 0; j < uniform_buffer_objects.size(); j++)
        {
            poolSizes[i].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            poolSizes[i].descriptorCount =
                static_cast<uint32_t>(VulkanConstants::max_in_flight);

            i++;
        }

        for(int j = 0; j < samplers.size(); j++)
        {
            poolSizes[i].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            poolSizes[i].descriptorCount =
                static_cast<uint32_t>(VulkanConstants::max_in_flight);
            i++;
        }

        VkDescriptorPoolCreateInfo poolInfo {};
        poolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes    = poolSizes.data();
        poolInfo.maxSets       = static_cast<uint32_t>(VulkanConstants::max_in_flight);

        if(vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) !=
           VK_SUCCESS)
            throw std::runtime_error("failed to create descriptor pool!");

        // Create DescriptorSets

        std::vector<VkDescriptorSetLayout> layouts(VulkanConstants::max_in_flight,
                                                   descriptorSetLayout);

        VkDescriptorSetAllocateInfo allocInfo {};
        allocInfo.sType          = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount =
            static_cast<uint32_t>(VulkanConstants::max_in_flight);
        allocInfo.pSetLayouts = layouts.data();

        descriptorSets.resize(VulkanConstants::max_in_flight);

        if(vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) !=
           VK_SUCCESS)
            throw std::runtime_error("failed to allocate descriptor sets!");

        for(size_t i = 0; i < VulkanConstants::max_in_flight; i++)
        {
            // I can preshot the size but well see that later on
            std::vector<VkWriteDescriptorSet> descriptorWrites {};

            int c = 0;

            for(const auto& uniform : uniform_buffer_objects)
            {
                VkDescriptorBufferInfo bufferInfo {};
                bufferInfo.buffer = uniform.uniformBuffers[i];
                bufferInfo.offset = 0;
                // So this is weird
                bufferInfo.range = sizeof(UniformBufferObject);

                VkWriteDescriptorSet descriptorWrite;

                descriptorWrite.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrite.dstSet          = descriptorSets[i];
                descriptorWrite.dstBinding      = 0;
                descriptorWrite.dstArrayElement = 0;
                descriptorWrite.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                descriptorWrite.descriptorCount = 1;
                descriptorWrite.pBufferInfo     = &bufferInfo;

                c++;
            }

            for(const auto& sampler : samplers)
            {
                VkDescriptorImageInfo imageInfo {};
                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

                imageInfo.imageView = sampler.imgview.textureImageView;
                imageInfo.sampler   = sampler;

                VkWriteDescriptorSet descriptorWrite;
                descriptorWrite.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrite.dstSet          = descriptorSets[i];
                descriptorWrite.dstBinding      = static_cast<uint32_t>(c);
                descriptorWrite.dstArrayElement = 0;
                descriptorWrite.descriptorType =
                    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                descriptorWrite.descriptorCount = 1;
                descriptorWrite.pImageInfo      = &imageInfo;
                c++;
            }

            vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()),
                                   descriptorWrites.data(), 0, nullptr);
        }
    }

private:
};
}    // namespace corgi