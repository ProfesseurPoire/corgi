#pragma once

#include <corgi/ui/Widget.h>

namespace corgi::ui
{
    /**
     * @brief Lines up widget vertically
     * 
     *        The Vertical Layout has 2 functionning mode : 
     * 
     *        * StretchLayoutToFillContent : The layout will use the widget's
     *          height and grow accordingly
     * 
     *        * StretchContentToFillLayout : The layout will set the widget's
     *          height depending on the layout size and the widget count
     */
    class VerticalLayout : public Widget
    {
    public:
        VerticalLayout() {}

        [[nodiscard]] float height() const noexcept override;

        void update(float elapsed_time) override;
        void paint(Renderer& renderer) override;

        float margin {0.0f};

        enum class StretchMode
        {
            StretchLayoutToFillContent,
            StretchContentToFillLayout
        };

        void setStretchMode(StretchMode mode);
        void setSpacing(float spacing);

        [[nodiscard]] float       getSpacing() const noexcept;
        [[nodiscard]] StretchMode getStretchMode() const noexcept;

    private:
        StretchMode mStretchMode = StretchMode::StretchLayoutToFillContent;

        float mSpacing = 0.0f;
    };
}    // namespace corgi::ui