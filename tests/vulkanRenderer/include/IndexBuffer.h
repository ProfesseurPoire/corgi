#pragma once

#include <vulkan/vulkan.hpp>

#include <vector>

VkBuffer       indexBuffer;
VkDeviceMemory indexBufferMemory;

const std::vector<uint16_t> indices = {0, 1, 2, 2, 3, 0};

struct IndexBuffer
{
    static inline void create(VkQueue          queue,
                              VkCommandPool    commandPool,
                              VkDevice         device,
                              VkPhysicalDevice physical_device)
    {
        VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

        VkBuffer       stagingBuffer;
        VkDeviceMemory stagingBufferMemory;

        Buffer::create(
            device, physical_device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, indices.data(), (size_t)bufferSize);
        vkUnmapMemory(device, stagingBufferMemory);

        Buffer::create(
            device, physical_device, bufferSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

        Buffer::copy(queue, commandPool, device, stagingBuffer, indexBuffer, bufferSize);

        vkDestroyBuffer(device, stagingBuffer, nullptr);
        vkFreeMemory(device, stagingBufferMemory, nullptr);
    }
};