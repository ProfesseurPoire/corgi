#pragma once

#include <vulkan/vulkan_core.h>

#include <array>
#include <span>
#include <stdexcept>

struct Vertex
{
    float x, y, z;
    float r, g, b;
    float u, v;
};

struct VertexBuffer
{
    VkBuffer       buffer;
    VkDeviceMemory buffer_memory;

    static uint32_t findMemoryType(VkPhysicalDevice      physicalDevice,
                                   uint32_t              typeFilter,
                                   VkMemoryPropertyFlags properties)
    {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

        for(uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
        {
            if((typeFilter & (1 << i)) &&
               (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }

    static VertexBuffer create_vertex_buffer(VkPhysicalDevice  physicalDevice,
                                             VkDevice          device,
                                             std::span<Vertex> vert);

    static void destroy_buffer(VkDevice device, VkBuffer buffer)
    {
        vkDestroyBuffer(device, buffer, nullptr);
    }

    static VkVertexInputBindingDescription getBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription {};
        bindingDescription.binding   = 0;
        bindingDescription.stride    = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions {};

        attributeDescriptions[0].binding  = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format   = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset   = 0;

        attributeDescriptions[1].binding  = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format   = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset   = sizeof(float) * 3;

        attributeDescriptions[2].binding  = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format   = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset   = sizeof(float) * 6;

        return attributeDescriptions;
    }
};
