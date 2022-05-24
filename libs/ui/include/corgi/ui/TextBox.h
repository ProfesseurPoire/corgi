#pragma once

#include <corgi/ui/Rectangle.h>
#include <corgi/ui/Text.h>
#include <corgi/ui/Widget.h>

#include <functional>

namespace corgi::ui
{
    class TextBox : public Widget
    {
    public:
        TextBox(const std::string& text = "");
        TextBox(std::reference_wrapper<std::string> text);

        void init() override;

        [[nodiscard]] corgi::Event<std::string>& textUpdated();

        void update(float elapsedTime) override;

        void setText(const std::string& str);

        SimpleString text();

        /**
         * @brief   Sets the background color
         *          The background color is the rectangle behind
         *          the text
         */
        void setBackgroundColor(Color color);

        void setBackgroundMouseOverColor(Color color);

        void setTextColor(Color color);
        void setHorizontalAlignment(HorizontalAlignment align);

        void removeRectangle();

        corgi::ui::Rectangle& getRectangle();
        corgi::ui::Text&      textWidget();

        [[nodiscard]] float cursorFlashingTime() const noexcept;

        void setCursorFlashingTime(float value);

    private:
        void initializeText();
        void initializeRectangle();
        void initializeVerticalBar();
        void initializeSelectionRectangle();

        int findCursorIndex(int x, int y);

        bool isCursorOutOfBound(int x, int y);

        struct Point
        {
            float x = 0.0f;
            float y = 0.0f;
        };

        int cursorAtEndLine(int lineIndex);

        int cursorAtStartLine(int lineIndex);

        /**
         * @brief       Gets the line at which the cursor is currently standing
         * 
         * @param index 
         * @return int 
         */
        int cursorLinePosition(int index);

        /**
         * @brief   Creates the selection rectangle on the textbox
         * 
         *          x and y are the mouse position
         * 
         * @param x 
         * @param y 
         */
        void createSelectionRectangles(int x, int y);

        /**
         * @brief   Returns the cursor position at @a cursorIndex
         * 
         * @param cursorIndex 
         * @return Point 
         */
        Point cursorPosition(int cursorIndex);

        /**
         * @brief   Updates the position of the cursor in screen/world space
         * 
         *          Basically, it translates the cursorIndexPosition into x/y
         */
        void updateCursorRealPosition();

        corgi::Event<std::string> mTextUpdated;

        std::string  mLocalText = "";
        std::string& mText;

        Color mBackgroundColor {255, 255, 255, 255};
        Color mMouseOverBackgroundColor {230, 230, 230, 255};

        corgi::ui::Rectangle* mRectangle           = nullptr;
        corgi::ui::Text*      uiText_              = nullptr;
        corgi::ui::Rectangle* mVerticalBar         = nullptr;
        corgi::ui::Widget*    selectionRectangles_ = nullptr;

        float mElapsedTimeSinceLastVerticalTick = 0.0f;
        float cursorFlashingTime_               = 5.0f;

        int cursorIndexPosition_  = 0;
        int cursorLineIndex_      = 0;
        int secondSelectionIndex_ = 0;

        int startSelectionIndex_;
        int selectiongLength_;
    };
}    // namespace corgi::ui
