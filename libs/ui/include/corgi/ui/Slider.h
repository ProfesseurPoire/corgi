#pragma once

#include <corgi/rendering/Material.h>
#include <corgi/ui/Rectangle.h>
#include <corgi/ui/Widget.h>

namespace corgi::ui
{
    class Slider : public Widget
    {
    public:
        enum class Orientation
        {
            Vertical,
            Horizontal
        };

        Slider();
        void init() override;

        [[nodiscard]] float getMinValue() const { return mMinValue; }
        [[nodiscard]] float getMaxValue() const { return mMaxValue; }
        [[nodiscard]] float getCurrentValue() const { return mCurrentValue; }

        void setMinValue(float value);
        void setMaxValue(float value);
        void setCurrentValue(float value);

        void setOrientation(Slider::Orientation orientation);

        [[nodiscard]] Orientation getOrientation() const;

        void paint(Renderer& renderer) override;

        [[nodiscard]] corgi::Event<float>& onValueChanged();

    private:
        corgi::Event<float> mOnValueChanged;

        /**
             * @brief The actual thing you grab with the mouse
             */
        corgi::ui::Rectangle* mHandle;

        /**
             * @brief The bar the handle reside on
             */
        corgi::ui::Rectangle* mContainer;

        float mMinValue      = 0.0f;
        float mMaxValue      = 1.0f;
        float mCurrentValue  = 1.5f;
        float mContainerSize = 20.0f;

        corgi::Material mMaterial;
        float           mDragOffset  = 0.0f;
        Orientation     mOrientation = Orientation::Horizontal;
    };
}    // namespace corgi::ui