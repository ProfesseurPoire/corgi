#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

Image Image::create_texture_image(VkDevice           device,
                                  VkPhysicalDevice   physical_device,
                                  VkCommandPool      commandPool,
                                  VkQueue            graphicsQueue,
                                  const std::string& filepath)
{

    int          texWidth, texHeight, texChannels;
    stbi_uc*     pixels    = stbi_load(("resources/" + filepath).c_str(), &texWidth, &texHeight,
                                &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;

    if(!pixels)
    {
        throw std::runtime_error("failed to load texture image!");
    }

    VkBuffer       stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    Buffer::create(device, physical_device, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                       VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                   stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(device, stagingBufferMemory);

    Image image =
        create_image(device, physical_device, texWidth, texHeight,
                     VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
                     VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    transitionImageLayout(device, commandPool, graphicsQueue, image.image,
                          VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyBufferToImage(device, commandPool, graphicsQueue, stagingBuffer, image.image,
                      static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));

    transitionImageLayout(device, commandPool, graphicsQueue, image.image,
                          VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    stbi_image_free(pixels);
    return image;
}