#pragma once

#include <corgi/ui/TextEnums.h>
#include <corgi/utils/Color.h>
#include <corgi/utils/Event.h>

namespace corgi
{
    class Font;
}

namespace corgi::ui
{
    class StyleSheet
    {
        class MenuBar
        {
        public:
            [[nodiscard]] Color backgroundMenuColor() const noexcept;
            [[nodiscard]] Color menuTitleTextColor() const noexcept;
            [[nodiscard]] Color backgroundMenuMouseOverColor() const noexcept;

            void setBackgroundMenuColor(Color newColor);
            void setMenuTitleTextColor(Color newColor);
            void setBackgroundMenuMouseOverColorChanged(Color newColor);

            Event<Color>& backgroundMenuColorChanged();
            Event<Color>& menuTitleTextColorChanged();
            Event<Color>& backgroundMenuMouseOverColorChanged();

        private:
            Event<Color> backgroundMenuColorChanged_;
            Event<Color> menuTitleTextColorChanged_;
            Event<Color> backgroundMenuMouseOverColorChanged_;

            Color backgroundMenuColor_          = Color(20, 20, 30);
            Color menuTitleTextColor_           = Color(220, 220, 220);
            Color backgroundMenuMouseOverColor_ = Color(90, 90, 135);
        } menuBar_;
        class DialogWindow
        {
        public:
            /**
             * @brief Gets the height of the title bar 
             * 
             * @return float 
             */
            [[nodiscard]] float titleBarHeight() const noexcept;

            /**
             * @brief   Gets the color used by the text in the title bar
             * 
             * @return  Color 
             */
            [[nodiscard]] Color titleTextColor() const noexcept;

            /**
             * @brief Gets the font used by the text in the title bar
             * 
             *        Warning : nullptr by default. Must be set at startup
             * 
             * @return Font* 
             */
            [[nodiscard]] Font* titleFont() const noexcept;

            /**
             * @brief Gets the background color used by the title bar 
             * 
             * @return Color 
             */
            [[nodiscard]] Color titleBackgroundColor() const noexcept;

            /**
             * @brief Gets the color used by the "content" of the widget
             *         
             * (Basically the area under the title bar)
             * 
             * @return Color 
             */
            [[nodiscard]] Color contentBackgroundColor() const noexcept;

            /**
             * @brief   Set the title's bar height
             * 
             *          This function will trigger the titleBarHeightChanged event
             * 
             * @param newHeight 
             */
            void setTitleBarHeight(float newHeight);

            /**
             * @brief   Sets the background color used by the title bar
             * 
             *          This function will trigger the titleBackgroundColorChanged event 
             * 
             * @param color 
             */
            void setTitlebackgroundColor(Color color);

            /**
             * @brief   Sets the color of the text displayed in the title bar
             * 
             *          This function triggers the titleTextColorChanged event
             * 
             * @param newColor 
             */
            void setTitleTextColor(Color newColor);

            /**
             * @brief   Set the font used by the title in the title bar
             * 
             *          This function will trigger the titleFontChanged event
             * 
             * @param font 
             */
            void setTitleFont(Font* newFont);

            /**
             * @brief   Sets the background color used by the content widget
             * 
             *          This function will trigger the contentBackgroundColorChanged event
             */
            void setContentBackgroundColorChanged(Color newColor);

            /**
             * @brief Gets the event that triggers when the content backrgound color is changed
             * 
             * @return Event<Color>& 
             */
            Event<Color>& contentBackgroundColorChanged();

            /**
             * @brief Gets the event that triggers when the title bar background color is changed
             * 
             * @return Event<Color>&
             */
            Event<Color>& titleBackgroundColorChanged();

            /**
             * @brief Gets the event that triggers when the font used by the title text is changed
             * 
             * @return Event<Font*>& 
             */
            Event<Font*>& titleFontChanged();

            /**
             * @brief   Gets the event that triggers when the color used by text in the title bar is changed
             * 
             * @return  Event<Color>& 
             */
            Event<Color>& titleTextColorChanged();

            /**
             * @brief Gets the event that triggers when the title's bar height is changed
             * 
             * @return Event<float>& 
             */
            Event<float>& titleBarHeightChanged();

        private:
            Color titleBackgroundColor_ {74, 87, 93};
            Color contentBackgroundColor_ {63, 70, 75};
            Color titleTextColor_ {200, 200, 200};
            Font* titleFont_      = nullptr;
            float titleBarHeight_ = 40.0f;

            Event<Color> titleBackgroundColorChanged_;
            Event<Color> titleTextColorChanged_;
            Event<Color> contentBackgroundColorChanged_;
            Event<Font*> titleFontChanged_;
            Event<float> titleBarHeightChanged_;

        } dialog_;

        class Button
        {
        public:
            /**
             * @brief   Gets the color displayed by the button's 
             * 
             * @return Color 
             */
            [[nodiscard]] Color textColorDeactivated() const noexcept;

            /**
             * @brief Gets the color displayed by the button background by default
             * 
             * @return Color 
             */
            [[nodiscard]] Color backgroundColor() const noexcept;

            /**
             * @brief Gets the color displayed by the button background when the mouse hover the button
             * 
             * @return Color 
             */
            [[nodiscard]] Color backgroundColorOnMouseEnter() const noexcept;

            /**
             * @brief Gets the color displayed by the button background after being pushed
             * 
             * @return Color 
             */
            [[nodiscard]] Color backgroundColorPushed() const noexcept;

            /**
             * @brief Gets the color used by the text inside the button
             * 
             * @return Color 
             */
            [[nodiscard]] Color textColor() const noexcept;

            /**
             * @brief Gets the color used for when the Button is deactivated
             * 
             * @return Color 
             */
            [[nodiscard]] Color backgroundColorDeactivated() const noexcept;

            /**
             * @brief Gets the corner radius of the button's box
             * 
             * @return float 
             */
            [[nodiscard]] float backgroundRadius() const noexcept;

            /**
             * @brief Gets the standard height of a button
             * 
             * @return float 
             */
            [[nodiscard]] float height() const noexcept;

            /**
             * @brief Gets the font used by the button
             * 
             * @return Font* 
             */
            [[nodiscard]] Font* font() const noexcept;

            /**
             * @brief Gets the horizontal alignment used by the button's text
             * 
             * @return HorizontalAlignment 
             */
            [[nodiscard]] HorizontalAlignment horizontalAlignment() const noexcept;

            /**
             * @brief Gets the vertical alignment used by the button's text
             * 
             * @return VerticalAlignment 
             */
            [[nodiscard]] VerticalAlignment verticalAlignment() const noexcept;

            void setHorizontalAlignment(HorizontalAlignment horizontalAlignment);
            void setVerticalAlignment(VerticalAlignment verticalAlignment);

            void setHeight(float height);
            void setFont(Font* font);

            void setTextColor(Color newColor);
            void setTextColorDeactivated(Color newColor);

            void setBackgroundColorPushed(Color newColor);
            void setBackgroundColorDeactivated(Color newColor);
            void setBackgroundColor(Color newColor);
            void setBackgroundColorOnMouseEnterChanged(Color newColor);

            void setBackgroundRadius(float radius);

            [[nodiscard]] Event<float>& heightChanged();
            [[nodiscard]] Event<float>& backgroundRadiusChanged();

            [[nodiscard]] Event<Color>& backgroundColorChanged();
            [[nodiscard]] Event<Color>& backgroundColorOnMouseEnterChanged();
            [[nodiscard]] Event<Color>& backgroundColorDeactivatedChanged();
            [[nodiscard]] Event<Color>& backgroundColorPushedChanged();

            [[nodiscard]] Event<Color>& textColorChanged();
            [[nodiscard]] Event<Color>& textColorDeactivatedChanged();

            [[nodiscard]] Event<Font*>& fontChanged();

            [[nodiscard]] Event<HorizontalAlignment>& horizontalAlignmentChanged();
            [[nodiscard]] Event<VerticalAlignment>&   verticalAlignmentChanged();

        private:
            Event<Color> backgroundColorChanged_;
            Event<Color> backgroundColorOnMouseEnterChanged_;
            Event<Color> backgroundColorPushedChanged_;
            Event<Color> backgroundColorDeactivatedChanged_;
            Event<Color> textColorChanged_;
            Event<Color> textColorDeactivatedChanged_;
            Event<float> backgroundRadiusChanged_;
            Event<float> heightChanged_;
            Event<Font*> fontChanged_;

            Event<corgi::HorizontalAlignment> horizontalAlignmentChanged_;
            Event<corgi::VerticalAlignment>   verticalAlignmentChanged_;

            Color backgroundColor_ {48, 54, 65};
            Color backgroundColorOnMouseEnter_ {90, 95, 95};
            Color backgroundColorDeactivated_ {150, 150, 150};
            // By defaut it uses the same color has the mouse enter
            Color backgroundColorPushed_ = backgroundColorOnMouseEnter_;

            Color textColor_ {255, 255, 255};
            Color textColorDeactivated_ {200, 200, 200};

            float backgroundRadius_ {20};
            float height_ {40};

            corgi::HorizontalAlignment horizontalAlignment_ {
                corgi::HorizontalAlignment::Center};
            corgi::VerticalAlignment verticalAlignment_ {
                corgi::VerticalAlignment::Center};

            // Default font is initialized by the Game class after
            // ResourceCache initialization
            Font* font_ = nullptr;

        } button_;

    public:
        /**
         * @brief Returns styling options for every button
         * 
         * @return Button& 
         */
        Button& button() { return button_; }

        /**
         * @brief Returns a reference 
         * 
         * @return DialogWindow& 
         */
        DialogWindow& dialog() { return dialog_; }

        /**
         * @brief Returns styling options for menuBars
         * 
         * @return MenuBar& 
         */
        MenuBar& menuBar() { return menuBar_; }
    };

    /**
     * @brief This is the default style sheet used when a new widget is created
     */
    inline StyleSheet defaultStyleSheet;

}    // namespace corgi::ui