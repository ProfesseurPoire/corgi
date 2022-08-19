#pragma once

#include "Image.h"
#include "Swapchain.h"
#include "Texture.h"

#include <vulkan/vulkan_core.h>

struct DepthBuffer
{
    static inline VkImage        depthImage;
    static inline VkDeviceMemory depthImageMemory;
    static inline VkImageView    depthImageView;

    static VkFormat findSupportedFormat(VkDevice                     device,
                                        VkPhysicalDevice             physical_device,
                                        const std::vector<VkFormat>& candidates,
                                        VkImageTiling                tiling,
                                        VkFormatFeatureFlags         features)
    {

        for(VkFormat format : candidates)
        {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(physical_device, format, &props);

            if(tiling == VK_IMAGE_TILING_LINEAR &&
               (props.linearTilingFeatures & features) == features)
            {
                return format;
            }
            else if(tiling == VK_IMAGE_TILING_OPTIMAL &&
                    (props.optimalTilingFeatures & features) == features)
            {
                return format;
            }
        }
        throw std::runtime_error("failed to find supported format!");
    }

    static bool hasStencilComponent(VkFormat format)
    {
        return format == VK_FORMAT_D32_SFLOAT_S8_UINT ||
               format == VK_FORMAT_D24_UNORM_S8_UINT;
    }

    static VkFormat findDepthFormat(VkDevice device, VkPhysicalDevice physical_device)
    {
        return findSupportedFormat(device, physical_device,
                                   {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT,
                                    VK_FORMAT_D24_UNORM_S8_UINT},
                                   VK_IMAGE_TILING_OPTIMAL,
                                   VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    }

    static void createDepthResources(Swapchain        swapchain,
                                     VkDevice         device,
                                     VkPhysicalDevice physical_device)
    {
        VkFormat depthFormat = findDepthFormat(device, physical_device);
        Image::create_image(
            device, physical_device, swapchain.create_info.imageExtent.width,
            swapchain.create_info.imageExtent.height, depthFormat,
            VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory);
        depthImageView = Texture::createImageView(device, depthImage, depthFormat,
                                                  VK_IMAGE_ASPECT_DEPTH_BIT);
    }
};