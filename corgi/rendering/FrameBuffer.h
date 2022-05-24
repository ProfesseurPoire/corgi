#pragma once

#include <corgi/containers/UniquePtr.h>

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
        UniquePtr<Texture> color_attachment_;
        UniquePtr<Texture> depth_buffer_;
    };
}    // namespace corgi