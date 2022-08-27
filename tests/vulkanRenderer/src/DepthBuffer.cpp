#include "DepthBuffer.h"


void DepthBuffer::initialize(Swapchain        swapchain,
                         VkDevice         device,
                         VkPhysicalDevice physical_device)
{
    const VkFormat depth_format = findDepthFormat(physical_device);

    // Our Depth Buffer is an image and needs to be the same resolution as the color attachment
    depth_image=  Image::create_image(
        device, 
        physical_device,
        swapchain.create_info.imageExtent.width,
        swapchain.create_info.imageExtent.height, 
        depth_format,
        VK_IMAGE_TILING_OPTIMAL,                        // TODO This feels a bit weird because if we don't get a format with optimal tiling that might not work
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,    // Telling vulkan the image will be used as a depth stencil attachment
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    // Creating an image view and telling Vulkan it's going to be used as a depth buffer
    depthImageView = ImageView::createImageView(device, depth_image.image, depth_format,
                                              VK_IMAGE_ASPECT_DEPTH_BIT);
}