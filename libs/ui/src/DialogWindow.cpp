#include <corgi/ui/DialogWindow.h>
#include <corgi/ui/SimpleButton.h>

using namespace corgi;
using namespace corgi::ui;

DialogWindow::DialogWindow(StyleSheet& styleSheet)
{
    setStyleSheet(styleSheet);
}

void DialogWindow::setStyleSheet(StyleSheet& styleSheet)
{
    disconnectStyleSheetEvents();
    styleSheet_ = &styleSheet;
    connectStyleSheetEvents();
}

void DialogWindow::disconnectStyleSheetEvents()
{
    if(!styleSheet_)
        return;

    styleSheet_->dialog().titleBackgroundColorChanged() -=
        keyTitleBackgroundColorChangedCallback_;
    styleSheet_->dialog().contentBackgroundColorChanged() -=
        keyContentBackgroundColorChangedCallback_;
    styleSheet_->dialog().titleFontChanged() -= keyTitleFontChangedCallback_;
    styleSheet_->dialog().titleTextColorChanged() -= keyTitleTextColorChangedCallback_;
    styleSheet_->dialog().titleBarHeightChanged() -= keyTitleBarHeightChangedCallback_;
}

void DialogWindow::connectStyleSheetEvents()
{
    keyTitleBarHeightChangedCallback_ = styleSheet_->dialog().titleBarHeightChanged() +=
        [&](float newHeight)
    {
        titleWidget_->setHeight(newHeight);
        titleRectangle_->setHeight(newHeight);
        contentWidget_->setTop(newHeight);
    };

    keyTitleTextColorChangedCallback_ = styleSheet_->dialog().titleTextColorChanged() +=
        [&](Color newColor)
    {
        titleWidget_->setColor(newColor);
    };

    keyTitleBackgroundColorChangedCallback_ =
        styleSheet_->dialog().titleBackgroundColorChanged() += [&](Color newColor)
    {
        titleRectangle_->setColor(newColor);
    };

    keyContentBackgroundColorChangedCallback_ =
        styleSheet_->dialog().contentBackgroundColorChanged() += [&](Color newColor)
    {
        background_->setColor(newColor);
    };

    keyTitleFontChangedCallback_ = styleSheet_->dialog().titleFontChanged() +=
        [&](Font* newFont)
    {
        titleWidget_->setFont(*newFont);
    };
}

void DialogWindow::init()
{
    background_ = &emplace_back<Rectangle>();
    background_->setAnchorsToFillParentSpace();
    background_->setTop(0.0f);
    background_->setColor(styleSheet_->dialog().contentBackgroundColor());
    background_->setRadius(0);

    initializeTitle();
    initializeContentWidget();
}

void DialogWindow::initializeContentWidget()
{
    contentWidget_ = &emplace_back<Widget>();
    contentWidget_->setTop(styleSheet_->dialog().titleBarHeight());

    contentWidget_->setLeft(0);
    contentWidget_->setRight(0);

    contentWidget_->setBottom(10 + styleSheet_->button().height() + 10);
}

Widget& DialogWindow::contentWidget()
{
    return *contentWidget_;
}

void DialogWindow::initializeTitle()
{
    titleRectangle_ = &emplace_back<Rectangle>();
    titleRectangle_->setLeft(0.0f);
    titleRectangle_->setRight(0.0f);
    titleRectangle_->setHeight(styleSheet_->dialog().titleBarHeight());
    titleRectangle_->setColor(styleSheet_->dialog().titleBackgroundColor());
    titleRectangle_->setRadius(0);

    titleWidget_ = &emplace_back<Text>();
    titleWidget_->setLeft(0);
    titleWidget_->setRight(0);
    titleWidget_->setHeight(styleSheet_->dialog().titleBarHeight());
    titleWidget_->setHorizontalAlignment(HorizontalAlignment::Left);
    titleWidget_->setMarginLeft(20);
    titleWidget_->setFont(styleSheet_->dialog().titleFont());
    titleWidget_->setText("Hello There");
    titleWidget_->setColor(styleSheet_->dialog().titleTextColor());

    titleWidget_->mouse_drag_start_event() += [&](const Mouse& mouse)
    {
        if(!isResizing)
        {
            mouseDragXPosition_ = mouse.x();
            mouseDragYPosition_ = mouse.y();

            positionAtStartDragX_ = real_x();
            positionAtStartDragY_ = real_y();
        }
    };

    titleWidget_->mouse_drag_event() += [&](const Mouse& mouse)
    {
        if(!isResizing)
        {
            setLeft(positionAtStartDragX_ + (mouse.x() - mouseDragXPosition_));
            setTop(positionAtStartDragY_ + (mouse.y() - mouseDragYPosition_));
        }
    };
}
