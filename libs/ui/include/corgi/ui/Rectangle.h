#pragma once

#include <corgi/math/easing.h>
#include <corgi/rendering/Material.h>
#include <corgi/ui/Widget.h>
#include <corgi/utils/Color.h>

namespace corgi::ui
{
    class Rectangle : public Widget
    {
    public:
        // Lifecycle

        Rectangle();

        Rectangle(const Rectangle& other) = default;
        Rectangle(Rectangle&& other)      = default;

        Rectangle& operator=(const Rectangle& other) = default;
        Rectangle& operator=(Rectangle&& other) = default;

        // Virtual functions

        void paint(Renderer& renderer);

        void setColorEasing(const Color&           dest,
                            easing::EasingEquation equation,
                            float                  duration);

        // Functions

        [[nodiscard]] Color color() const;

        /**
         * @brief   Sets the rectangle's color
         * 
         *          Also deasctivate a Color Easing if one was active
         * 
         *          The reason we don't allow for setColor(int,int,int,int) functions is
         *          because I'd always have to add the code that make sure the values
         *          given are well within range
         * 
         * @param color 
         */
        void setColor(const Color& color);

        [[nodiscard]] float radius() const;
        void                setRadius(float radius);

        void setDepth(float depth) { depth_ = depth; }

    private:
        Color mColor;
        float mRadius {0.0f};
        float mElapsedTimeEasing {0.0f};

        Color mDestinationColor;
        Color mStartColor;

        easing::EasingEquation mEasingEquation;
        float                  mDuration {0.0f};
        bool                   mColorEasing = false;

        float depth_ = 0.0f;
    };
}    // namespace corgi::ui
