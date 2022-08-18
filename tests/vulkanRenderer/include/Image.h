#pragma once

#define STB_IMAGE_IMPLEMENTATION

#include "Buffer.h"
#include "stb_image.h"

#include <vulkan/vulkan_core.h>

struct Image
{
    static inline VkImage        textureImage;
    static inline VkDeviceMemory textureImageMemory;
    static inline VkBuffer       stagingBuffer;
    static inline VkDeviceMemory stagingBufferMemory;

    static inline VkPipelineStageFlags sourceStage;
    static inline VkPipelineStageFlags destinationStage;

    static void copyBufferToImage(VkDevice      device,
                                  VkCommandPool commandPool,
                                  VkQueue       graphicsQueue,
                                  VkBuffer      buffer,
                                  VkImage       image,
                                  uint32_t      width,
                                  uint32_t      height)
    {
        VkCommandBuffer commandBuffer =
            Buffer::beginSingleTimeCommands(commandPool, device);

        VkBufferImageCopy region {};
        region.bufferOffset      = 0;
        region.bufferRowLength   = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel       = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount     = 1;

        region.imageOffset = {0, 0, 0};
        region.imageExtent = {width, height, 1};

        vkCmdCopyBufferToImage(commandBuffer, buffer, image,
                               VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

        Buffer::endSingleTimeCommands(graphicsQueue, commandPool, device, commandBuffer);
    }

    static void transitionImageLayout(VkDevice      device,
                                      VkCommandPool commandPool,
                                      VkQueue       graphicsQueue,
                                      VkImage       image,
                                      VkFormat      format,
                                      VkImageLayout oldLayout,
                                      VkImageLayout newLayout)
    {
        VkCommandBuffer commandBuffer =
            Buffer::beginSingleTimeCommands(commandPool, device);

        VkImageMemoryBarrier barrier {};
        barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout                       = oldLayout;
        barrier.newLayout                       = newLayout;
        barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        barrier.image                           = image;
        barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel   = 0;
        barrier.subresourceRange.levelCount     = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount     = 1;

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if(oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
           newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage      = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if(oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
                newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage      = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else
        {
            throw std::invalid_argument("unsupported layout transition!");
        }

        vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr,
                             0, nullptr, 1, &barrier);

        Buffer::endSingleTimeCommands(graphicsQueue, commandPool, device, commandBuffer);
    }

    static void create_image(VkDevice              device,
                             VkPhysicalDevice      physical_device,
                             uint32_t              width,
                             uint32_t              height,
                             VkFormat              format,
                             VkImageTiling         tiling,
                             VkImageUsageFlags     usage,
                             VkMemoryPropertyFlags properties,
                             VkImage&              image,
                             VkDeviceMemory&       imageMemory)
    {
        VkImageCreateInfo imageInfo {};
        imageInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType     = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width  = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth  = 1;
        imageInfo.mipLevels     = 1;
        imageInfo.arrayLayers   = 1;
        imageInfo.format        = format;
        imageInfo.tiling        = tiling;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage         = usage;
        imageInfo.samples       = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;

        if(vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create image!");
        }

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(device, image, &memRequirements);

        VkMemoryAllocateInfo allocInfo {};
        allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize  = memRequirements.size;
        allocInfo.memoryTypeIndex = Buffer::findMemoryType(
            physical_device, memRequirements.memoryTypeBits, properties);

        if(vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate image memory!");
        }

        vkBindImageMemory(device, image, imageMemory, 0);
    }

    static void create_texture_image(VkDevice         device,
                                     VkPhysicalDevice physical_device,
                                     VkCommandPool    commandPool,
                                     VkQueue          graphicsQueue)
    {

        int          texWidth, texHeight, texChannels;
        stbi_uc*     pixels    = stbi_load("resources/corgi.png", &texWidth, &texHeight,
                                           &texChannels, STBI_rgb_alpha);
        VkDeviceSize imageSize = texWidth * texHeight * 4;

        if(!pixels)
        {
            throw std::runtime_error("failed to load texture image!");
        }

        VkBuffer       stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        Buffer::create(
            device, physical_device, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
        memcpy(data, pixels, static_cast<size_t>(imageSize));
        vkUnmapMemory(device, stagingBufferMemory);

        create_image(device, physical_device, texWidth, texHeight,
                     VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
                     VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage,
                     textureImageMemory);

        transitionImageLayout(device, commandPool, graphicsQueue, textureImage,
                              VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED,
                              VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        copyBufferToImage(device, commandPool, graphicsQueue, stagingBuffer, textureImage,
                          static_cast<uint32_t>(texWidth),
                          static_cast<uint32_t>(texHeight));

        transitionImageLayout(device, commandPool, graphicsQueue, textureImage,
                              VK_FORMAT_R8G8B8A8_SRGB,
                              VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        stbi_image_free(pixels);
    }
};