#include <corgi/filesystem/FileSystem.h>
#include <corgi/rendering/vulkan/VulkanShader.h>
#include <vulkan/vulkan.hpp>

using namespace corgi;

VulkanShader::VulkanShader(VkDevice device, Stage stage, const std::string& file)
    : Shader(stage)
{
    const auto buffer = corgi::filesystem::read_file_binary(file);

    VkShaderModuleCreateInfo createInfo {};
    createInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = buffer.size();
    createInfo.pCode    = reinterpret_cast<const uint32_t*>(buffer.data());

    if(vkCreateShaderModule(device, &createInfo, nullptr, &shader_module_) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create shader module " + file);
    }
}

VulkanShader::~VulkanShader()
{
    vkDestroyShaderModule(device_, shader_module_, nullptr);
}
