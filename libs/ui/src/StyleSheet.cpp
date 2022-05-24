#include <corgi/ui/StyleSheet.h>

using namespace corgi::ui;
using namespace corgi;

Event<Color>& StyleSheet::MenuBar::backgroundMenuColorChanged()
{
    return backgroundMenuColorChanged_;
}

Event<Color>& StyleSheet::MenuBar::backgroundMenuMouseOverColorChanged()
{
    return backgroundMenuMouseOverColorChanged_;
}

Event<Color>& StyleSheet::MenuBar::menuTitleTextColorChanged()
{
    return menuTitleTextColorChanged_;
}

Color StyleSheet::MenuBar::backgroundMenuMouseOverColor() const noexcept
{
    return backgroundMenuMouseOverColor_;
}

Color StyleSheet::MenuBar::backgroundMenuColor() const noexcept
{
    return backgroundMenuColor_;
}

Color StyleSheet::MenuBar::menuTitleTextColor() const noexcept
{
    return menuTitleTextColor_;
}

void StyleSheet::MenuBar::setBackgroundMenuMouseOverColorChanged(Color newColor)
{
    if(backgroundMenuMouseOverColor_ != newColor)
    {
        backgroundMenuMouseOverColorChanged_(newColor);
        backgroundMenuMouseOverColor_ = newColor;
    }
}
void StyleSheet::MenuBar::setMenuTitleTextColor(Color newColor)
{
    if(menuTitleTextColor_ != newColor)
    {
        menuTitleTextColor_ = newColor;
        menuTitleTextColorChanged_(newColor);
    }
}

void StyleSheet::MenuBar::setBackgroundMenuColor(Color newColor)
{
    if(backgroundMenuColor_ != newColor)
    {
        backgroundMenuColor_ = newColor;
        backgroundMenuColorChanged_(newColor);
    }
}

float StyleSheet::DialogWindow::titleBarHeight() const noexcept
{
    return titleBarHeight_;
}

void StyleSheet::DialogWindow::setTitleBarHeight(float newHeight)
{
    if(titleBarHeight_ != newHeight)
    {
        titleBarHeight_ = newHeight;
        titleBarHeightChanged_(newHeight);
    }
}

Event<float>& StyleSheet::DialogWindow::titleBarHeightChanged()
{
    return titleBarHeightChanged_;
}

Color StyleSheet::DialogWindow::titleTextColor() const noexcept
{
    return titleTextColor_;
}

Event<Color>& StyleSheet::DialogWindow::titleTextColorChanged()
{
    return titleTextColorChanged_;
}

void StyleSheet::DialogWindow::setTitleTextColor(Color newColor)
{
    if(titleTextColor_ != newColor)
    {
        titleTextColor_ = newColor;
        titleTextColorChanged_(newColor);
    }
}

Font* StyleSheet::DialogWindow::titleFont() const noexcept
{
    return titleFont_;
}

void StyleSheet::DialogWindow::setTitleFont(Font* newFont)
{
    if(titleFont_ != newFont)
    {
        titleFont_ = newFont;
        titleFontChanged_(newFont);
    }
}

Event<Font*>& StyleSheet::DialogWindow::titleFontChanged()
{
    return titleFontChanged_;
}

void StyleSheet::DialogWindow::setTitlebackgroundColor(Color newColor)
{
    if(titleBackgroundColor_ != newColor)
    {
        titleBackgroundColor_ = newColor;
        titleBackgroundColorChanged_(newColor);
    }
}

void StyleSheet::DialogWindow::setContentBackgroundColorChanged(Color newColor)
{
    if(contentBackgroundColor_ != newColor)
    {
        contentBackgroundColor_ = newColor;
        contentBackgroundColorChanged_(newColor);
    }
}

Color StyleSheet::DialogWindow::contentBackgroundColor() const noexcept
{
    return contentBackgroundColor_;
}

Event<Color>& StyleSheet::DialogWindow::contentBackgroundColorChanged()
{
    return contentBackgroundColorChanged_;
}

Event<Color>& StyleSheet::DialogWindow::titleBackgroundColorChanged()
{
    return titleBackgroundColorChanged_;
}

Color StyleSheet::DialogWindow::titleBackgroundColor() const noexcept
{
    return titleBackgroundColor_;
}

corgi::HorizontalAlignment StyleSheet::Button::horizontalAlignment() const noexcept
{
    return horizontalAlignment_;
}

void StyleSheet::Button::setHorizontalAlignment(HorizontalAlignment horizontalAlignment)
{
    if(horizontalAlignment_ != horizontalAlignment)
    {
        horizontalAlignment_ = horizontalAlignment;
        horizontalAlignmentChanged_(horizontalAlignment);
    }
}

void StyleSheet::Button::setVerticalAlignment(VerticalAlignment verticalAlignment)
{
    if(verticalAlignment_ != verticalAlignment)
    {
        verticalAlignment_ = verticalAlignment;
        verticalAlignmentChanged_(verticalAlignment);
    }
}

corgi::VerticalAlignment StyleSheet::Button::verticalAlignment() const noexcept
{
    return verticalAlignment_;
}

Font* StyleSheet::Button::font() const noexcept
{
    return font_;
}

Event<HorizontalAlignment>& StyleSheet::Button::horizontalAlignmentChanged()
{
    return horizontalAlignmentChanged_;
}

Event<VerticalAlignment>& StyleSheet::Button::verticalAlignmentChanged()
{
    return verticalAlignmentChanged_;
}

Event<Font*>& StyleSheet::Button::fontChanged()
{
    return fontChanged_;
}

void StyleSheet::Button::setFont(Font* font)
{
    if(font_ != font)
    {
        font_ = font;
        fontChanged_(font);
    }
}

Event<Color>& StyleSheet::Button::backgroundColorPushedChanged()
{
    return backgroundColorPushedChanged_;
}

void StyleSheet::Button::setBackgroundColorPushed(Color newColor)
{
    if(backgroundColorPushed_ != newColor)
    {
        backgroundColorPushed_ = newColor;
        backgroundColorChanged_(newColor);
    }
}

Color StyleSheet::Button::backgroundColorPushed() const noexcept
{
    return backgroundColorPushed_;
}

void StyleSheet::Button::setTextColor(Color newColor)
{
    if(newColor != textColor_)
    {
        textColor_ = newColor;
        textColorChanged_(newColor);
    }
}

float StyleSheet::Button::height() const noexcept
{
    return height_;
}

void StyleSheet::Button::setHeight(float height)
{
    if(height_ != height)
    {
        height_ = height;
        heightChanged_(height);
    }
}

Event<float>& StyleSheet::Button::heightChanged()
{
    return heightChanged_;
}

Event<Color>& StyleSheet::Button::textColorChanged()
{
    return textColorChanged_;
}

Color StyleSheet::Button::textColor() const noexcept
{
    return textColor_;
}

void StyleSheet::Button::setBackgroundColor(Color newColor)
{
    if(backgroundColor_ != newColor)
    {
        backgroundColor_ = newColor;
        backgroundColorChanged_(newColor);
    }
}

void StyleSheet::Button::setBackgroundRadius(float radius)
{
    if(radius != backgroundRadius_)
    {
        backgroundRadius_ = radius;
        backgroundRadiusChanged_(radius);
    }
}

Event<float>& StyleSheet::Button::backgroundRadiusChanged()
{
    return backgroundRadiusChanged_;
}

float StyleSheet::Button::backgroundRadius() const noexcept
{
    return backgroundRadius_;
}

Color StyleSheet::Button::backgroundColorOnMouseEnter() const noexcept
{
    return backgroundColorOnMouseEnter_;
}

Color StyleSheet::Button::backgroundColor() const noexcept
{
    return backgroundColor_;
}

Event<Color>& StyleSheet::Button::backgroundColorChanged()
{
    return backgroundColorChanged_;
}

Event<Color>& StyleSheet::Button::backgroundColorOnMouseEnterChanged()
{
    return backgroundColorOnMouseEnterChanged_;
}

void StyleSheet::Button::setBackgroundColorOnMouseEnterChanged(Color newColor)
{
    if(backgroundColorOnMouseEnter_ != newColor)
    {
        backgroundColorOnMouseEnter_ = newColor;
        backgroundColorOnMouseEnterChanged_(newColor);
    }
}

void StyleSheet::Button::setBackgroundColorDeactivated(Color newColor)
{
    if(backgroundColorDeactivated_ != newColor)
    {
        backgroundColorDeactivated_ = newColor;
        backgroundColorDeactivatedChanged_(newColor);
    }
}

Event<Color>& StyleSheet::Button::backgroundColorDeactivatedChanged()
{
    return backgroundColorDeactivatedChanged_;
}

Color StyleSheet::Button::backgroundColorDeactivated() const noexcept
{
    return backgroundColorDeactivated_;
}

Color StyleSheet::Button::textColorDeactivated() const noexcept
{
    return textColorDeactivated_;
}

void StyleSheet::Button::setTextColorDeactivated(Color newColor)
{
    if(textColorDeactivated_ != newColor)
    {
        textColorDeactivated_ = newColor;
        textColorDeactivatedChanged_(newColor);
    }
}

Event<Color>& StyleSheet::Button::textColorDeactivatedChanged()
{
    return textColorDeactivatedChanged_;
}