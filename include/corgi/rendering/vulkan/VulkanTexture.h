#pragma once

#include <corgi/rendering/AbstractTexture.h>

#include <corgi/rendering/vulkan/Image.h>
#include <corgi/rendering/vulkan/ImageView.h>

#include <vulkan/vulkan_core.h>

namespace corgi
{
class VulkanTexture  : public AbstractTexture
{
public:

	Vulkan::Image image;
	VkImageView image_view;

	VulkanTexture(AbstractTexture::CreateInfo info,
                      VkDevice                    device,
                      VkPhysicalDevice            physical_device,
                      VkCommandPool               commandPool,
                      VkQueue                     graphicsQueue) 
	{
        VkFormat format;

		switch(info.format)
        {
            case AbstractTexture::Format::RGBA:
                format = VK_FORMAT_R8G8B8A8_SRGB;

            default:
                break;
        }

        VkDeviceSize   imageSize = info.width * info.height * 4;

        VkBuffer       stagingBuffer;
        VkDeviceMemory stagingBufferMemory;

        Buffer::create(
            device, physical_device, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
        memcpy(data, info.data, static_cast<size_t>(imageSize));
        vkUnmapMemory(device, stagingBufferMemory);

		image = Vulkan::Image::create_image(device, physical_device, info.width, info.height, format,
                            VK_IMAGE_TILING_OPTIMAL,
                            VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        Vulkan::Image::transitionImageLayout(device, commandPool, graphicsQueue, image.image,
                                      VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED,
                                      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        Vulkan::Image::copyBufferToImage(device, commandPool, graphicsQueue, stagingBuffer,
                            image.image, static_cast<uint32_t>(info.width),
                            static_cast<uint32_t>(info.height));

        Vulkan::Image::transitionImageLayout(device, commandPool, graphicsQueue, image.image,
                                VK_FORMAT_R8G8B8A8_SRGB,
                                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);


        VkImageViewCreateInfo viewInfo {};
        viewInfo.sType                         = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image                         = image.image;
        viewInfo.viewType                      = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format                        = format;
        viewInfo.subresourceRange.aspectMask   = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount   = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount     = 1;

        if(vkCreateImageView(device, &viewInfo, nullptr, &image_view) != VK_SUCCESS)
            throw std::runtime_error("failed to create texture image view!");
	}

private:
};
}    // namespace corgi