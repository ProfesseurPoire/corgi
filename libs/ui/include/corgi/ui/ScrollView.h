#pragma once

#include <corgi/rendering/Material.h>
#include <corgi/ui/Rectangle.h>
#include <corgi/ui/Widget.h>
#include <corgi/utils/Color.h>

namespace corgi::ui
{
    class Slider;
    class ScrollView : public Widget
    {
    public:
        ScrollView();

        void init() override;

        void paint(corgi::Renderer& renderer) override;
        void actual_paint(corgi::Renderer& renderer) override;

        Color handleColor {0.4f, 0.44f, 0.48f, 1.0f};
        Color selectedHandleColor {0.7f, 0.73f, 0.78f, 1.0f};
        Color containerColor {0.3f, 0.35f, 0.4f};

        Widget* content();

    private:
        Rectangle* mVerticalScrollbarContainer = nullptr;
        Rectangle* mVerticalScrollbarHandle    = nullptr;
        Widget*    content_                    = nullptr;

        /*
         * @brief   Sets the scrollbarOffset by making sure it stays in a valid range
         */
        void setScrollbarOffset(float scrollbarOffset);

        bool mHeightScrollingBar = false;
        bool mWidthScrollingBar  = false;

        float scrollbarOffset = 0.0f;

        float startDragY  = 0.0f;
        float startOffset = 0.0f;

        bool mIsDragging = false;

        corgi::Material mMaterial;
    };
}    // namespace corgi::ui