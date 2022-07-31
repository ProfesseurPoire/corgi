#pragma once

#include <memory>

namespace corgi
{
    class Texture;

    class FrameBuffer
    {
        friend class Renderer;

    public:
        // Lifecycle

        /*!
		 * @brief	Creates a new FrameBuffer that contains both a 
		 *			texture to store the color and depth stencil 
		 *			information. The textures are of the given size
		 */
        FrameBuffer(int width, int height);

        FrameBuffer(const FrameBuffer& framebuffer);
        FrameBuffer(FrameBuffer&& framebuffer) noexcept;

        FrameBuffer& operator=(const FrameBuffer& framebuffer);
        FrameBuffer& operator=(FrameBuffer&& framebuffer) noexcept;

        ~FrameBuffer();

        // Functions

        [[nodiscard]] Texture* color_attachment() const;
        [[nodiscard]] Texture* depth_stencil() const;

        [[nodiscard]] int width() const noexcept;
        [[nodiscard]] int height() const noexcept;

    private:
        // Functions

        void init();

        // Variables

        int                width_ {0};
        int                height_ {0};
        unsigned int       id_ {0};
        std::unique_ptr<Texture> color_attachment_;
        std::unique_ptr<Texture> depth_buffer_;
    };
}    // namespace corgi