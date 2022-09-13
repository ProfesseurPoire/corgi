#pragma once

#include <corgi/rendering/AbstractRenderer.h>
#include <corgi/rendering/vulkan/VulkanMaterial.h>
#include <corgi/rendering/vulkan/VulkanUniformBufferObject.h>
#include <vulkan/vulkan.hpp>

namespace corgi
{

class VulkanRenderer : public AbstractRenderer
{
public:
    VulkanRenderer();
    ~VulkanRenderer() override {}

    VkDevice         device_;
    VkPhysicalDevice physical_device_;

    Texture create_texture();
    void    delete_texture(Texture texture);

    void draw(const Mesh& mesh, const Material& material) {}

    /**
     * @brief   Create a ubo object that will send what's inside the data pointer to 
     *          the GPU
     * @param data 
     * @param size 
     * @param shader_stage 
     * @return corgi::UniformBufferObject* 
     */
    corgi::UniformBufferObject*
    create_ubo(void* data, int size, UniformBufferObject::ShaderStage shader_stage)
    {
        return ubos_
            .emplace_back(new VulkanUniformBufferObject(device_, physical_device_, data,
                                                        size, shader_stage))
            .get();
    }

    corgi::VulkanMaterial* create_material(UniformBufferObject::ShaderStage shader_stage)
    {
        return new VulkanMaterial(device_, physical_device_);
    }

private:
};
}    // namespace corgi