#include <corgi/rendering/VulkanRenderer.h>
#include <corgi/rendering/vulkan/VulkanMaterial.h>

using namespace corgi;

VulkanRenderer::VulkanRenderer() {}

Texture VulkanRenderer::create_texture()
{
    return Texture();
}

void VulkanRenderer::delete_texture(Texture texture) {}
