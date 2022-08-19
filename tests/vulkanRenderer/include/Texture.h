#pragma once

#include "Image.h"

#include <vulkan/vulkan_core.h>

struct Texture
{
    static inline VkImageView textureImageView;
    static inline VkSampler   textureSampler;

    static VkImageView createImageView(VkDevice           device,
                                       VkImage            image,
                                       VkFormat           format,
                                       VkImageAspectFlags aspectFlags)
    {
        VkImageViewCreateInfo viewInfo {};
        viewInfo.sType                         = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image                         = image;
        viewInfo.viewType                      = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format                        = format;
        viewInfo.subresourceRange.aspectMask   = aspectFlags;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount   = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount     = 1;

        VkImageView imageView;
        if(vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create texture image view!");
        }

        return imageView;
    }

    static void createTextureSampler(VkDevice device, VkPhysicalDevice physical_device)
    {
        VkSamplerCreateInfo samplerInfo {};
        samplerInfo.sType     = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;

        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

        samplerInfo.anisotropyEnable = VK_TRUE;
        //samplerInfo.maxAnisotropy    = ? ? ? ;

        VkPhysicalDeviceProperties properties {};
        vkGetPhysicalDeviceProperties(physical_device, &properties);

        samplerInfo.maxAnisotropy           = properties.limits.maxSamplerAnisotropy;
        samplerInfo.borderColor             = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable           = VK_FALSE;
        samplerInfo.compareOp               = VK_COMPARE_OP_ALWAYS;

        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod     = 0.0f;
        samplerInfo.maxLod     = 0.0f;

        if(vkCreateSampler(device, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create texture sampler!");
        }
    }

    static void create_texture_image_view(VkDevice device)
    {
        textureImageView =
            createImageView(device, Image::textureImage, VK_FORMAT_R8G8B8A8_SRGB,
                            VK_IMAGE_ASPECT_COLOR_BIT);
    }
};