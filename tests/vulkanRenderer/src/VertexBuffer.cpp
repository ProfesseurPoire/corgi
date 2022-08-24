#include "VertexBuffer.h"

VertexBuffer VertexBuffer::create_vertex_buffer(VkPhysicalDevice  physicalDevice,
                                      VkDevice          device,
                                                std::span<Vertex> vert)
{
    VertexBuffer vertex;


    VkBufferCreateInfo bufferInfo {};
    bufferInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size        = sizeof(vert[0]) * vert.size();
    bufferInfo.usage       = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if(vkCreateBuffer(device, &bufferInfo, nullptr, &vertex.buffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create vertex buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, vertex.buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo {};
    allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize  = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(
        physicalDevice, memRequirements.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    if(vkAllocateMemory(device, &allocInfo, nullptr, &vertex.buffer_memory) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate vertex buffer memory!");
    }
    vkBindBufferMemory(device, vertex.buffer, vertex.buffer_memory, 0);
    void* data;
    vkMapMemory(device, vertex.buffer_memory, 0, bufferInfo.size, 0, &data);
    memcpy(data, vert.data(), (size_t)bufferInfo.size);
    vkUnmapMemory(device, vertex.buffer_memory);

    return vertex;
}