#pragma once

#include <corgi/rendering/AbstractRenderer.h>
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

    void                 draw(const Mesh& mesh, const Material& material) {}
    UniformBufferObject* create_ubo(UniformBufferObject::ShaderStage shader_stage);

private:
};
}    // namespace corgi