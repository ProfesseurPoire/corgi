#pragma once

#include <corgi/resources/Shader.h>

// TODO : Maybe I should try to hide the vulkan header?
#include <vulkan/vulkan.hpp>

namespace corgi
{
    class VulkanShader : public Shader
    {
    public:

        VulkanShader(VkDevice device, Stage stage, const std::string& file);
        ~VulkanShader();

        VkShaderModule shader_module_;
        VkDevice device_;
    };
}