#pragma once

#include <vulkan/vulkan_core.h>
#include <stdexcept>

/*!
 * @brief   The framebuffer is basically just the image we're going to draw on
 * 
 *          Thus its main properties are its dimensions width and height.
 *          And the imageview?
 */
class Framebuffer 
{
public:
    // Not a huge fan
    Framebuffer() = default;

    Framebuffer (   unsigned     width,
                    unsigned     height,
                                     VkImageView* attachments,
                                     VkRenderPass render_pass,
                                     VkDevice     device)
    {

        create_info_.sType      = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        create_info_.renderPass = render_pass;
        create_info_.attachmentCount = 1;
        create_info_.pAttachments    = attachments;
        create_info_.width           = width;
        create_info_.height          = height;
        create_info_.layers          = 1;

        if(vkCreateFramebuffer(device, &create_info_, nullptr,
                               &framebuffer_) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }

	[[nodiscard]] const VkFramebufferCreateInfo& create_info() const { return create_info_; }
    [[nodiscard]] const VkFramebuffer& framebuffer() const { return framebuffer_; }

    private:

	VkFramebufferCreateInfo create_info_ {};
        VkFramebuffer           framebuffer_ {};

};