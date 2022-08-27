#include "UniformBufferObject.h"

#include "VulkanConstants.h"

void UniformBufferObject::destroy_descriptor_set_layout(VkDevice device)
{
    vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
}

void UniformBufferObject::create_descriptor_pool(VkDevice device)
{
    std::array<VkDescriptorPoolSize, 2> poolSizes {};
    poolSizes[0].type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(VulkanConstants::max_in_flight);

    poolSizes[1].type            = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(VulkanConstants::max_in_flight);

    VkDescriptorPoolCreateInfo poolInfo {};
    poolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes    = poolSizes.data();
    poolInfo.maxSets       = static_cast<uint32_t>(VulkanConstants::max_in_flight);

    if(vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
        throw std::runtime_error("failed to create descriptor pool!");
}

void UniformBufferObject::create_descriptor_sets(VkDevice device)
{
    std::vector<VkDescriptorSetLayout> layouts(VulkanConstants::max_in_flight,
                                               descriptorSetLayout);
    VkDescriptorSetAllocateInfo        allocInfo {};
    allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool     = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(VulkanConstants::max_in_flight);
    allocInfo.pSetLayouts        = layouts.data();

    descriptorSets.resize(VulkanConstants::max_in_flight);

    if(vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) != VK_SUCCESS)
        throw std::runtime_error("failed to allocate descriptor sets!");

    for(size_t i = 0; i < VulkanConstants::max_in_flight; i++)
    {
        VkDescriptorBufferInfo bufferInfo {};
        bufferInfo.buffer = uniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range  = sizeof(UniformBufferObject);

        VkDescriptorImageInfo imageInfo {};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView   = image_view.textureImageView;
        imageInfo.sampler     = sampler;

        std::array<VkWriteDescriptorSet, 2> descriptorWrites {};

        descriptorWrites[0].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet          = descriptorSets[i];
        descriptorWrites[0].dstBinding      = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo     = &bufferInfo;

        descriptorWrites[1].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet          = descriptorSets[i];
        descriptorWrites[1].dstBinding      = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo      = &imageInfo;

        vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()),
                               descriptorWrites.data(), 0, nullptr);
    }
}

void UniformBufferObject::createDescriptorSetLayout(VkDevice    device,
                                                    ShaderStage shader_stage,
                                                    int         layout)
{
    // Temporary for now
    const VkDescriptorSetLayoutBinding samplerLayoutBinding {
        .binding            = 1,
        .descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount    = 1,
        .stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT,
        .pImmutableSamplers = nullptr};

    VkDescriptorSetLayoutBinding uboLayoutBinding {};
    uboLayoutBinding.binding = layout;
    switch(shader_stage)
    {
        // Means it's the UBO for the Vertex Shader
        case ShaderStage::Fragment:
            uboLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
            break;

        // Means it's the UBO for the Vertex Shader
        case ShaderStage::Vertex:
            uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
            break;
    }

    uboLayoutBinding.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount    = 1;
    uboLayoutBinding.pImmutableSamplers = nullptr;    // Optional

    std::array<VkDescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding,
                                                            samplerLayoutBinding};
    VkDescriptorSetLayoutCreateInfo             layoutInfo {};
    layoutInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings    = bindings.data();

    if(vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) !=
       VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

void UniformBufferObject::add_uniform(VkDevice         device,
                                      VkPhysicalDevice physical_device,
                                      void*            data,
                                      int              size,
                                      ShaderStage      shader_stage,
                                      int              layout,
                                      ImageView        image_view,
                                      VkSampler        sampler)
{

    this->image_view    = image_view;
    this->sampler       = sampler;

    createDescriptorSetLayout(device, shader_stage, layout);

    uniforms.emplace_back(data, size, shader_stage, layout);

    uniformBuffers.resize(VulkanConstants::max_in_flight);
    uniformBuffersMemory.resize(VulkanConstants::max_in_flight);

    for(size_t i = 0; i < VulkanConstants::max_in_flight; i++)
    {
        // When creating the buffer I need to know the size it's going to take
        // and the BufferMemory is where we're going to actually store the data.
        // So the buffer points to the BufferMemory
        Buffer::create(device, physical_device, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                       uniformBuffers[i], uniformBuffersMemory[i]);
    }
}

void UniformBufferObject::update(VkDevice device, uint32_t currentImage)
{
    void* data;
    vkMapMemory(device, uniformBuffersMemory[currentImage], 0, uniforms.front().size, 0,
                &data);
    memcpy(data, uniforms.front().data, uniforms.front().size);
    vkUnmapMemory(device, uniformBuffersMemory[currentImage]);
}