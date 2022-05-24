#pragma once

#include <corgi/rendering/Material.h>
#include <corgi/ui/Widget.h>

namespace corgi::ui
{
    class Text;

    class Button : public Widget
    {
    public:
        // Lifecycle

        Button()                    = default;
        ~Button() override          = default;
        Button(const Button& other) = default;
        Button(Button&& other)      = default;

        Button& operator=(const Button& other) = default;
        Button& operator=(Button&& other) = default;

        // Virtual functions

        void init() override;
        void paint(Renderer& renderer) override;
        void update(float elapsed_time) override;

        // Accessors

        [[nodiscard]] Text& text() noexcept { return *text_; }

        [[nodiscard]] Event<>& on_click() { return on_click_; }
        [[nodiscard]] Event<>& on_update() { return on_update_; }

    protected:
        Text* text_;

        bool is_pressed_ {false};

        // TODO : I probably want to send the mouse information?
        Event<> on_click_;
        Event<> on_update_;

        Material mMaterial;
    };
}    // namespace corgi::ui