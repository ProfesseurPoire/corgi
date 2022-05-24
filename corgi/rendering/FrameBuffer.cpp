#include "FrameBuffer.h"

#include <corgi/logger/log.h>
#include <corgi/rendering/RenderCommand.h>

namespace corgi
{
    FrameBuffer::FrameBuffer(const int width, const int height)
        : width_(width)
        , height_(height)
    {
        id_ = RenderCommand::generate_frame_buffer();
        init();
    }

    FrameBuffer::FrameBuffer(const FrameBuffer& framebuffer)
    {
        width_  = framebuffer.width_;
        height_ = framebuffer.height_;
        id_     = RenderCommand::generate_frame_buffer();
        init();
    }

    FrameBuffer::FrameBuffer(FrameBuffer&& framebuffer) noexcept
        : width_(framebuffer.width_)
        , height_(framebuffer.height_)
        , id_(framebuffer.id_)
        , color_attachment_(std::move(framebuffer.color_attachment_))
        , depth_buffer_(std::move(framebuffer.depth_buffer_))
    {
        // Just to make sure the moved Framebuffer is in an empty state
        framebuffer.id_     = 0;
        framebuffer.width_  = 0;
        framebuffer.height_ = 0;
    }

    FrameBuffer& FrameBuffer::operator=(const FrameBuffer& framebuffer)
    {
        width_  = framebuffer.width_;
        height_ = framebuffer.height_;
        id_     = RenderCommand::generate_frame_buffer();
        init();
        return *this;
    }

    FrameBuffer& FrameBuffer::operator=(FrameBuffer&& framebuffer) noexcept
    {
        width_            = framebuffer.width_;
        height_           = framebuffer.height_;
        id_               = framebuffer.id_;
        color_attachment_ = std::move(framebuffer.color_attachment_);
        depth_buffer_     = std::move(framebuffer.depth_buffer_);

        framebuffer.id_     = 0;
        framebuffer.width_  = 0;
        framebuffer.height_ = 0;
        return *this;
    }

    FrameBuffer::~FrameBuffer()
    {
        if(id_ != 0)
            RenderCommand::delete_frame_buffer(id_);
    }

    void FrameBuffer::init()
    {
        static SimpleString colorAttachmentString        = "color_attachment";
        static SimpleString depthStencilAttachmentString = "depth_stencil_attachment";

        // Sets the texture's color attachment
        color_attachment_ = make_unique<Texture>(
            "color_attachment", width_, height_, Texture::MinFilter::Nearest,
            Texture::MagFilter::Nearest, Texture::Wrap::Repeat, Texture::Wrap::Repeat,
            Texture::Format::RGBA, Texture::InternalFormat::RGBA,
            Texture::DataType::UnsignedByte);

        depth_buffer_ = make_unique<Texture>(
            "depth_stencil_attachment", width_, height_, Texture::MinFilter::Linear,
            Texture::MagFilter::Linear, Texture::Wrap::Repeat, Texture::Wrap::Repeat,
            Texture::Format::DEPTH_STENCIL, Texture::InternalFormat::DEPTH24_STENCIL8,
            Texture::DataType::UnsignedInt24_8);

        RenderCommand::bind_frame_buffer(id_);
        RenderCommand::set_color_attachement(color_attachment_->id());
        RenderCommand::set_depth_stencil(depth_buffer_->id());
        RenderCommand::bind_frame_buffer(0);
    }

    int FrameBuffer::width() const noexcept { return width_; }

    int FrameBuffer::height() const noexcept { return height_; }

    Texture* FrameBuffer::color_attachment() const { return color_attachment_.get(); }

    Texture* FrameBuffer::depth_stencil() const { return depth_buffer_.get(); }
}    // namespace corgi