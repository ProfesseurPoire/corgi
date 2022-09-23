#pragma once

#include <corgi/rendering/AbstractRenderer.h>
#include <corgi/rendering/vulkan/VulkanMaterial.h>
#include <corgi/rendering/vulkan/VulkanShader.h>
#include <corgi/rendering/vulkan/VulkanUniformBufferObject.h>
#include <corgi/rendering/vulkan/VulkanTexture.h>
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
    VkCommandPool command_pool;
    VkQueue         graphics_queue;
    RenderPass render_pass;
    Swapchain swapchain;

    Texture create_texture();
    void    delete_texture(Texture texture);

    // That way the shaders will get deleted when the thing gets destroyed
    std::vector<std::unique_ptr<VulkanShader>> shaders_;

    /**
     * \brief Creates a new shader by loading the given file
     * \param stage 
     * \param file 
     * \return 
     */
    Shader* create_shader(Shader::Stage stage, const std::string& file) override
    {
        return shaders_.emplace_back(std::make_unique<VulkanShader>(device_, stage, file))
            .get();
    }

    corgi::AbstractTexture* create_texture(AbstractTexture::CreateInfo info) 
    {
        return new VulkanTexture(info, device_, physical_device_, command_pool, graphics_queue);
    }

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

    Sampler* create_sampler(Sampler::CreateInfo info) override
    {
        return new VulkanSampler(info, device_, physical_device_);
    }

    corgi::UniformBufferObject* create_ubo(UniformBufferObject::CreateInfo info)
    {
        return ubos_
            .emplace_back(new VulkanUniformBufferObject(info, device_, physical_device_))
            .get();
    }

    corgi::VulkanMaterial* create_material(const std::string& vertexShader,
                                           const std::string& fragmenShader)
    {
        return new VulkanMaterial(device_, physical_device_);
    }

    corgi::VulkanMaterial*
    create_material(AbstractMaterial::Descriptor descriptor) override
    {
        return new VulkanMaterial(device_, physical_device_, descriptor, render_pass,
                                  swapchain);
    }

private:
};
}    // namespace corgi