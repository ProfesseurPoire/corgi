#pragma once

#include <corgi/ui/Rectangle.h>
#include <corgi/ui/StyleSheet.h>
#include <corgi/ui/Text.h>
#include <corgi/ui/Widget.h>

namespace corgi::ui
{

    class DialogWindow : public Widget
    {
    public:
        DialogWindow(StyleSheet& stylesheet = defaultStyleSheet);
        void init() override;

        void setStyleSheet(StyleSheet& styleSheet);

        /**
         * @brief   Returns the widget storing the content of the DialogWindow
         * 
         *          Basically, this is where you want to add your own things to
         *          the DialogWindow
         * 
         * @return  Widget& 
         */
        Widget& contentWidget();

    protected:
        StyleSheet* styleSheet_ {nullptr};

    private:
        void initializeTitle();
        void initializeContentWidget();
        void connectStyleSheetEvents();
        void disconnectStyleSheetEvents();

        Rectangle* background_;
        Rectangle* titleRectangle_ {nullptr};
        Text*      titleWidget_ {nullptr};
        Widget*    contentWidget_ {nullptr};

        float mouseDragXPosition_;
        float mouseDragYPosition_;

        float positionAtStartDragX_;
        float positionAtStartDragY_;

        int keyTitleBackgroundColorChangedCallback_;
        int keyContentBackgroundColorChangedCallback_;
        int keyTitleFontChangedCallback_;
        int keyTitleTextColorChangedCallback_;
        int keyTitleBarHeightChangedCallback_;
    };

}    // namespace corgi::ui