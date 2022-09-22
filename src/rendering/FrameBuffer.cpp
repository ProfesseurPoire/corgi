#include <corgi/rendering/FrameBuffer.h>
#include <corgi/rendering/RenderCommand.h>
#include <corgi/rendering/Sampler.h>

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
    static std::string colorAttachmentString        = "color_attachment";
    static std::string depthStencilAttachmentString = "depth_stencil_attachment";

    // Sets the texture's color attachment
    color_attachment_ = std::make_unique<Texture>(
        "color_attachment", width_, height_, AbstractTexture::Format::RGBA,
        AbstractTexture::InternalFormat::RGBA, AbstractTexture::DataType::UnsignedByte);

    depth_buffer_ = std::make_unique<Texture>("depth_stencil_attachment", width_, height_,
                                              AbstractTexture::Format::DEPTH_STENCIL,
                                  AbstractTexture::InternalFormat::DEPTH24_STENCIL8,
                                              AbstractTexture::DataType::UnsignedInt24_8);

    RenderCommand::bind_frame_buffer(id_);
    RenderCommand::set_color_attachement(color_attachment_->id());
    RenderCommand::set_depth_stencil(depth_buffer_->id());
    RenderCommand::bind_frame_buffer(0);
}

int FrameBuffer::width() const noexcept
{
    return width_;
}

int FrameBuffer::height() const noexcept
{
    return height_;
}

Texture* FrameBuffer::color_attachment() const
{
    return color_attachment_.get();
}

Texture* FrameBuffer::depth_stencil() const
{
    return depth_buffer_.get();
}
}    // namespace corgi
