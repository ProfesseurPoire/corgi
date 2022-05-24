#pragma once

#include <corgi/ui/Widget.h>

namespace corgi
{
    class Texture;
}

namespace corgi::ui
{
    class Panel : public Widget
    {
    public:
        // Lifecycle

        Panel()  = default;
        ~Panel() = default;

        Panel(const Panel& other) = default;
        Panel(Panel&& other)      = default;

        Panel& operator=(const Panel& other) = default;
        Panel& operator=(Panel&& other) = default;

        // Virtual functions

        void paint(Renderer& renderer) override;
        void update(float elapsed_time) override;

        Texture* texture {nullptr};
    };
}    // namespace corgi::ui