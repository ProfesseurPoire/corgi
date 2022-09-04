#include "IndexBuffer.h"

#include <corgi/rendering/vulkan/Buffer.h>

void IndexBuffer::create(const VkQueue                   queue,
                         const VkCommandPool             commandPool,
                         const VkDevice                  device,
                         const VkPhysicalDevice          physical_device,
                         const std::span<const uint16_t> indexes)
{
    const VkDeviceSize bufferSize = sizeof(indexes[0]) * indexes.size();

    VkBuffer       stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    Buffer::create(device, physical_device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                       VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                   stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indexes.data(), bufferSize);
    vkUnmapMemory(device, stagingBufferMemory);

    Buffer::create(device, physical_device, bufferSize,
                   VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

    Buffer::copy(queue, commandPool, device, stagingBuffer, indexBuffer, bufferSize);

    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);

    size = indexes.size();
}