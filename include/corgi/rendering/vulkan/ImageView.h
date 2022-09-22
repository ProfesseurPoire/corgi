#pragma once

#include "Image.h"

#include <vulkan/vulkan_core.h>

struct ImageView
{
    VkImageView textureImageView;

    static VkImageView createImageView(VkDevice           device,
                                VkImage            image,
                                VkFormat           format,
                                VkImageAspectFlags aspectFlags);

    static VkSampler createTextureSampler(VkDevice         device,
                                               VkPhysicalDevice physical_device);

    static ImageView create_texture_image_view(Vulkan::Image image, VkDevice device);
};