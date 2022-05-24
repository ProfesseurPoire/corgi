#pragma once

#include <corgi/SimpleString.h>
#include <corgi/ui/StyleSheet.h>
#include <corgi/ui/Widget.h>
#include <corgi/utils/Event.h>

namespace corgi::ui
{
    class Text;
    class Rectangle;

    class SimpleButton : public Widget
    {
    public:
        SimpleButton(StyleSheet& styleSheet = defaultStyleSheet);

        void init() override;

        /**
         * @brief   Gets the textWidget
         * 
         * @return Text& 
         */
        Text& textWidget();

        /**
         * @brief Sets the button's text
         * 
         * @param text Text displayed by the button
         */
        void setText(const SimpleString& text);

        /**
         * @brief Returns a const reference to the stylesheet used by the button
         * 
         * @return const StyleSheet& 
         */
        [[nodiscard]] const StyleSheet& styleSheet() const;

        /**
         * @brief Sets the Style Sheet object
         * 
         * @param styleSheet const reference to the style applied to the button
         */
        void setStyleSheet(StyleSheet& styleSheet);

        /**
         * @brief Override the height of the button
         * 
         *        By default we use the height defined in the StyleSheet. 
         *        But by calling this function, we can override it invidually
         * 
         * @param height 
         */
        void setHeight(float height);

    private:
        void connectStyleSheetEvents();
        void initializeStyleSheet();

        StyleSheet* styleSheet_;
        Rectangle*  background_;
        Text*       text_;
        bool        mouseEntered_ = false;
        float       height_       = -1.0f;
    };
}    // namespace corgi::ui