#pragma once

#include <corgi/rendering/Sampler.h>
#include <corgi/rendering/vulkan/ImageView.h>
#include <corgi/rendering/vulkan/VulkanConstants.h>
#include <vulkan/vulkan.hpp>

namespace corgi
{
class VulkanSampler : public Sampler
{
public:
    VkSampler sampler;

    VkDescriptorPool      descriptorPool;
    VkDescriptorSetLayout descriptorSetLayout;
    VkPipelineLayout      pipelineLayout;

    VkDescriptorSetLayoutBinding bindingLayout;
    ImageView imgview;

    void init(VkDevice device, ImageView image_view)
    {
        // Notice that I might need to specify the shader
        // Tells on which binding this sampler is
        const VkDescriptorSetLayoutBinding samplerLayoutBinding 
        {
            .binding            = static_cast<uint32_t>(binding),
            .descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount    = 1,
            .stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT,
            .pImmutableSamplers = nullptr
            };

        // All of the above is clearly material stuff and not "Sampler" or Uniform stuff

        // This is probably something that will have to be moved to material
        // later on but one step at a time
        VkDescriptorSetLayoutCreateInfo layoutInfo {};
        layoutInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = 1;
        layoutInfo.pBindings    = &samplerLayoutBinding;

        if(vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr,
                                       &descriptorSetLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor set layout!");
        }

        // Create Descriptor Pool
        // Here it's already kinda material related
        // since we should make a juge poolSize to hold every uniform

        VkDescriptorPoolSize poolSizes {};
        poolSizes.type            = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes.descriptorCount = static_cast<uint32_t>(VulkanConstants::max_in_flight);

        VkDescriptorPoolCreateInfo poolInfo {};
        poolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(1);
        poolInfo.pPoolSizes    = &poolSizes;
        poolInfo.maxSets       = static_cast<uint32_t>(VulkanConstants::max_in_flight);

        // create descriptor sets

        // So we make a DescriptorSetLayout for each potential frame in flight
        std::vector<VkDescriptorSetLayout> layouts(VulkanConstants::max_in_flight,
                                                   descriptorSetLayout);

        VkDescriptorSetAllocateInfo allocInfo {};
        allocInfo.sType          = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount =
            static_cast<uint32_t>(VulkanConstants::max_in_flight);
        allocInfo.pSetLayouts = layouts.data();

        std::vector<VkDescriptorSet> descriptorSets;
        descriptorSets.resize(VulkanConstants::max_in_flight);

        if(vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) !=
           VK_SUCCESS)
            throw std::runtime_error("failed to allocate descriptor sets!");

        // So this part here is to actually write in the descriptor set
        // This is where we bind our ImageView to the Binding
        for(size_t i = 0; i < VulkanConstants::max_in_flight; i++)
        {
            VkDescriptorImageInfo imageInfo {};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView   = image_view.textureImageView;
            imageInfo.sampler     = sampler;

            VkWriteDescriptorSet descriptorWrites {};

            descriptorWrites.sType  = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites.dstSet = descriptorSets[i];
            // I find it a bit weird that we have to tell which binding twice?
            descriptorWrites.dstBinding = 0;

            descriptorWrites.dstArrayElement = 0;
            descriptorWrites.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites.descriptorCount = 1;
            descriptorWrites.pImageInfo      = &imageInfo;

            vkUpdateDescriptorSets(device, static_cast<uint32_t>(1), &descriptorWrites, 0,
                                   nullptr);
        }
    }

private:
};
}    // namespace corgi