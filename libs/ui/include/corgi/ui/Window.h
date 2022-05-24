#pragma once
#include <corgi/SimpleString.h>
#include <corgi/math/rectanglef.h>
#include <corgi/rendering/DrawList.h>
#include <corgi/ui/Widget.h>

namespace corgi::ui
{
    class Window : public Widget
    {
    public:
        enum class Resize
        {
            N,
            S,
            E,
            W,
            NE,
            NW,
            SE,
            SW,
            None
        };

        using Rect = math::Rectanglef;

        // Lifecycle

        Window();

        // Functions

        void update_rectangles();

        void paint(Renderer& renderer) override;

        void title(const SimpleString& title) { title_ = title; }

        [[nodiscard]] const SimpleString& title() const noexcept { return title_; }

    private:
        DrawList::NineSlice ns;

        Mesh* _mesh;

        Rect left_border_;
        Rect right_border_;
        Rect bottom_border_;
        Rect top_border_;
        Rect top_left_corner_;
        Rect top_right_corner_;
        Rect bottom_left_corner_;
        Rect bottom_right_corner_;
        Rect title_bar_;

        Font* font {nullptr};

        float _texture_width;
        float _texture_height;

        Resize resize_ {Resize::None};

        float base_width_;
        float base_height_;

        float base_x_;
        float base_y_;

        float off_x_;
        float off_y_;

        float          resize_offset_x_ {0};
        float          resize_offset_y_ {0.0f};
        bool           move_title_bar_ {false};
        SimpleString   title_;
        DrawList::Text _text_title;
    };
}    // namespace corgi::ui
