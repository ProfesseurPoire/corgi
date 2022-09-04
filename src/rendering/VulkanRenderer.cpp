#include <corgi/rendering/VulkanRenderer.h>
#include <corgi/rendering/vulkan/VulkanUniformBufferObject.h>

using namespace corgi;

VulkanRenderer::VulkanRenderer() {}

Texture VulkanRenderer::create_texture()
{
    return Texture();
}

void VulkanRenderer::delete_texture(Texture texture) {}

UniformBufferObject*
VulkanRenderer::create_ubo(UniformBufferObject::ShaderStage shader_stage)
{
    //
    return new VulkanUniformBufferObject(device_, physical_device_, shader_stage);
}