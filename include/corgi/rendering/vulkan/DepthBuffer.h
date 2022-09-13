#pragma once

#include <corgi/rendering/vulkan/VulkanSwapchain.h>

#include <corgi/rendering/vulkan/Image.h>
#include <corgi/rendering/vulkan/ImageView.h>
#include <vulkan/vulkan_core.h>

struct DepthBuffer
{

    void
    initialize(Swapchain swapchain, VkDevice device, VkPhysicalDevice physical_device);

    Image       depth_image;
    VkImageView depthImageView;

    /**
     * \brief Select the "best" format to use for the depth buffer
     *
     * \param device 
     * \param physical_device 
     * \param candidates 
     * \param tiling 
     * \param features 
     * \return 
     */
    static VkFormat find_supported_format(VkPhysicalDevice             physical_device,
                                          const std::vector<VkFormat>& candidates,
                                          VkImageTiling                tiling,
                                          VkFormatFeatureFlags         features)
    {

        for(const VkFormat format : candidates)
        {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(physical_device, format, &props);

            // Tiling can either be linear or optimal.
            // Linear tiling means that texels are stored in memory in row major order
            // Optimal means it's implementation specific

            // We make sure that the format can be used as a depth buffer attachment by the GPU
            if(tiling == VK_IMAGE_TILING_LINEAR &&
               (props.linearTilingFeatures & features) == features)
                return format;

            if(tiling == VK_IMAGE_TILING_OPTIMAL &&
               (props.optimalTilingFeatures & features) == features)
                return format;
        }
        throw std::runtime_error("failed to find supported format!");
    }

    static bool hasStencilComponent(VkFormat format)
    {
        return format == VK_FORMAT_D32_SFLOAT_S8_UINT ||
               format == VK_FORMAT_D24_UNORM_S8_UINT;
    }

    static VkFormat findDepthFormat(VkPhysicalDevice physical_device)
    {
        // We need a format with _D for the depth buffer
        return find_supported_format(physical_device,
                                     {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT,
                                      VK_FORMAT_D24_UNORM_S8_UINT},
                                     VK_IMAGE_TILING_OPTIMAL,
                                     VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    }
};