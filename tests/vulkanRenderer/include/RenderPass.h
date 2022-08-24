#pragma once

#include "DepthBuffer.h"

#include <vulkan/vulkan.hpp>

/**
 * @brief   Tells vulkan on which attachment to draw on
 * 
 *          Divided in subpasses and dependencies
 *          The attachments must match the one in the framebuffer that will
 *          be tied to the render pass
 */
class RenderPass
{
public:
    VkRenderPassCreateInfo create_info;
    VkRenderPass           render_pass;

    void initialize(VkDevice device, VkPhysicalDevice physical_device, VkFormat format)
    {
        VkAttachmentDescription depthAttachment {};
        depthAttachment.format  = DepthBuffer::findDepthFormat(physical_device); // why do I use the find function here?
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp  = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentRef {};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription colorAttachment {};

        // Telling vulkan the format of cour color attachment
        colorAttachment.format = format;

        // Only 1 sample. This can be used for multisampling
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

        // Here we simply tell Vulkan to clear the color attachment when we start the render pass
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;

        // Here we simply tell vulkan to store our attachment
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

        // And other stuff we don't really need to know for now
        colorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;

        // We need to specify VK_IMAGE_LAYOUT_PRESENT_SRC_KHR to tell vulkan that
        // the color attachment can be used for to be rendered
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        // This is just a reference to our attachment we give to the subpasses
        VkAttachmentReference colorAttachmentRef {};
        colorAttachmentRef.attachment = 0;
        // Layout used by the subpass
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

        subpass.colorAttachmentCount    = 1;
        subpass.pColorAttachments       = &colorAttachmentRef;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;

        VkSubpassDependency dependency {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;

        dependency.srcAccessMask = 0;

        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                                  VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                                  VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                                   VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        std::array<VkAttachmentDescription, 2> attachments = {colorAttachment,
                                                              depthAttachment};
        VkRenderPassCreateInfo                 renderPassInfo {};
        renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments    = attachments.data();
        renderPassInfo.subpassCount    = 1;
        renderPassInfo.pSubpasses      = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies   = &dependency;

        if(vkCreateRenderPass(device, &renderPassInfo, nullptr, &render_pass) !=
           VK_SUCCESS)
        {
            throw std::runtime_error("failed to create render pass!");
        }
    }

private:
};