#include <corgi/main/Cursor.h>
#include <corgi/ui/Rectangle.h>
#include <corgi/ui/SimpleButton.h>
#include <corgi/ui/Text.h>

using namespace corgi::ui;
using namespace corgi;

SimpleButton::SimpleButton(StyleSheet& styleSheet)
    : styleSheet_(&styleSheet)
{
    connectStyleSheetEvents();
}

Text& SimpleButton::textWidget()
{
    return *text_;
}

const StyleSheet& SimpleButton::styleSheet() const
{
    return *styleSheet_;
}

void SimpleButton::setStyleSheet(StyleSheet& styleSheet)
{
    styleSheet_ = &styleSheet;
    connectStyleSheetEvents();
}

void SimpleButton::setHeight(float height)
{
    height_ = height;
    Widget::setHeight(height);
}

void SimpleButton::connectStyleSheetEvents()
{
    styleSheet_->button().horizontalAlignmentChanged() +=
        [&](HorizontalAlignment horizontalAlignment)
    {
        text_->setHorizontalAlignment(horizontalAlignment);
    };

    styleSheet_->button().verticalAlignmentChanged() +=
        [&](VerticalAlignment verticalAlignment)
    {
        text_->setVerticalAlignment(verticalAlignment);
    };

    styleSheet_->button().backgroundColorChanged() += [&](Color backgroundColor)
    {
        background_->setColor(backgroundColor);
    };

    styleSheet_->button().backgroundColorOnMouseEnterChanged() += [&](Color color)
    {
        if(mouseEntered_)
        {
            background_->setColor(color);
        }
    };

    styleSheet_->button().backgroundRadiusChanged() += [&](float radius)
    {
        background_->setRadius(radius);
    };

    styleSheet_->button().textColorChanged() += [&](Color color)
    {
        text_->setColor(color);
    };

    styleSheet_->button().heightChanged() += [&](float height)
    {
        // If the user manually set the height, we don't use the stylesheet
        // height value
        if(height_ <= 0.0f)
            setHeight(height);
    };

    styleSheet_->button().fontChanged() += [&](Font* font)
    {
        text_->setFont(*font);
    };
}

void SimpleButton::setText(const SimpleString& text)
{
    text_->setText(text);
}

void SimpleButton::initializeStyleSheet()
{
    text_->setColor(styleSheet_->button().textColor());
    background_->setRadius(styleSheet_->button().backgroundRadius());
    background_->setColor(styleSheet_->button().backgroundColor());
    text_->setFont(*styleSheet_->button().font());
    text_->setHorizontalAlignment(styleSheet_->button().horizontalAlignment());
    text_->setVerticalAlignment(styleSheet_->button().verticalAlignment());
    setHeight(styleSheet_->button().height());
}

void SimpleButton::init()
{
    background_ = &emplace_back<corgi::ui::Rectangle>();
    background_->setPropagateEvent(true);
    text_ = &background_->emplace_back<corgi::ui::Text>();
    text_->setPropagateEvent(true);

    initializeStyleSheet();

    text_->setAnchorsToFillParentSpace();

    background_->onMouseEnter() += [&](int x, int y)
    {
        mouseEntered_ = true;
        corgi::Cursor::set(corgi::Cursor::DefaultCursor::Hand);
        background_->setColorEasing(styleSheet_->button().backgroundColorOnMouseEnter(),
                                    easing::EasingEquation::QuadraticEasingOut, 0.20f);
    };

    background_->mouseDownEvent() += [&](const Mouse& mouse)
    {
        if(mouseEntered_)
        {
            background_->setColorEasing(styleSheet_->button().backgroundColorPushed(),
                                        easing::EasingEquation::QuadraticEasingOut,
                                        0.20f);
        }
    };

    background_->mouseUpEvent() += [&](const Mouse& mouse)
    {
        if(mouseEntered_)
        {
            background_->setColorEasing(
                styleSheet_->button().backgroundColorOnMouseEnter(),
                easing::EasingEquation::QuadraticEasingOut, 0.20f);
        }
    };

    background_->onMouseExit() += [=](int x, int y)
    {
        mouseEntered_ = false;
        corgi::Cursor::set(corgi::Cursor::DefaultCursor::Arrow);
        background_->setColorEasing(styleSheet_->button().backgroundColor(),
                                    easing::EasingEquation::QuadraticEasingOut, 0.20f);
    };

    background_->setAnchorsToFillParentSpace();

    onDeactivate_ += [&]()
    {
        background_->setColor(styleSheet_->button().backgroundColorDeactivated());
        text_->setColor(styleSheet_->button().textColorDeactivated());
    };

    onActivate_ += [&]()
    {
        background_->setColor(styleSheet_->button().backgroundColor());
        text_->setColor(styleSheet_->button().textColor());
    };
}
