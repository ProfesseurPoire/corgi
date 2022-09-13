#pragma once

#include <corgi/rendering/UniformBufferObject.h>
#include <corgi/rendering/vulkan/VulkanConstants.h>

#include <vector>

namespace corgi
{
class VulkanUniformBufferObject : public UniformBufferObject
{
public:
    VulkanUniformBufferObject(VkDevice         device,
                              VkPhysicalDevice physical_device,
                              void*            data,
                              int              size,
                              ShaderStage      shader_stage)
        : UniformBufferObject(shader_stage)
    {
        data_   = data;
        device_ = device;
        size_   = size;

        uniformBuffers.resize(VulkanConstants::max_in_flight);
        uniformBuffersMemory.resize(VulkanConstants::max_in_flight);

        for(size_t i = 0; i < VulkanConstants::max_in_flight; i++)
        {
            // When creating the buffer I need to know the size it's going to take
            // and the BufferMemory is where we're going to actually store the data.
            // So the buffer points to the BufferMemory
            Buffer::create(device, physical_device, size,
                           VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                               VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                           uniformBuffers[i], uniformBuffersMemory[i]);
        }
    }

    VkDevice                    device_;
    std::vector<VkBuffer>       uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;

    void set_data(void* data, int size) override {}
    void use() override {}
    void update(int image = 0) override
    {
        void* data;
        vkMapMemory(device_, uniformBuffersMemory[image], 0, size_, 0, &data);
        memcpy(data, data_, size_);
        vkUnmapMemory(device_, uniformBuffersMemory[image]);
    };
};
}    // namespace corgi