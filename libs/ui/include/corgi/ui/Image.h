#pragma once

#include <corgi/ui/Widget.h>

namespace corgi
{
    class Texture;
}

namespace corgi::ui
{
    class Image : public Widget
    {
    public:
        enum class Stretch
        {
            Fill,
            KeepAspectRatio
        };

        Image(Texture* image = nullptr);

        void set_image(Texture& texture) { texture_ = &texture; }
        void setImage(Texture* texture) { texture_ = texture; }
        void setImage(Texture& texture) { texture_ = &texture; }

        void setImage(const Texture& texture) { texture_ = &texture; }
        void setImage(const Texture* texture) { texture_ = texture; }

        void setStretch(Stretch stretch) { stretch_ = stretch; }

        /**
         * @brief   Gets the texture displayed by the Image Widget
         * 
         *          We return a pointer because the widget can be empty
         * 
         * @return const Texture* 
         */
        [[nodiscard]] const Texture* texture() const noexcept;

        void paint(Renderer& renderer) override;

    private:
        Stretch               stretch_ {Stretch::KeepAspectRatio};
        const corgi::Texture* texture_ = nullptr;
    };
}    // namespace corgi::ui