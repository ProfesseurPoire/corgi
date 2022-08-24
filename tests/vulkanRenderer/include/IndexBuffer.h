#pragma once

#include <vulkan/vulkan.hpp>

#include <span>

struct IndexBuffer
{
    VkBuffer       indexBuffer;
    VkDeviceMemory indexBufferMemory;
    int size;

    void create(VkQueue             queue,
                VkCommandPool       commandPool,
                VkDevice            device,
                VkPhysicalDevice    physical_device,
                std::span<const uint16_t> indexes);
};