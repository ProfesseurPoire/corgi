#include "Color.h"

#include <corgi/math/MathUtils.h>

using namespace corgi;

Color::Color(int r, int g, int b, int a) noexcept
    : red_(math::clamp(r, 0, 255))
    , green_(math::clamp(g, 0, 255))
    , blue_(math::clamp(b, 0, 255))
    , alpha_(math::clamp(a, 0, 255))
{
}

Color::Color(float r, float g, float b, float a) noexcept
    : red_(math::clamp(static_cast<int>(math::clamp(r, 0.0f, 1.0f) * 255.0f), 0, 255))
    , green_(math::clamp(static_cast<int>(math::clamp(g, 0.0f, 1.0f) * 255.0f), 0, 255))
    , blue_(math::clamp(static_cast<int>(math::clamp(b, 0.0f, 1.0f) * 255.0f), 0, 255))
    , alpha_(math::clamp(static_cast<int>(math::clamp(a, 0.0f, 1.0f) * 255.0f), 0, 255))
{
}

void Color::setRed(int red) noexcept
{
    red_ = math::clamp(red, 0, 255);
}

void Color::setRed(float red) noexcept
{
    red_ = math::clamp(static_cast<int>(math::clamp(red, 0.0f, 1.0f) * 255.0f), 0, 255);
}

void Color::setGreen(float green) noexcept
{
    green_ =
        math::clamp(static_cast<int>(math::clamp(green, 0.0f, 1.0f) * 255.0f), 0, 255);
}

void Color::setGreen(int green) noexcept
{
    green_ = math::clamp(green, 0, 255);
}

void Color::setBlue(float blue) noexcept
{
    blue_ = math::clamp(static_cast<int>(math::clamp(blue, 0.0f, 1.0f) * 255.0f), 0, 255);
}

void Color::setBlue(int blue) noexcept
{
    blue_ = math::clamp(blue, 0, 255);
}

void Color::setAlpha(float alpha) noexcept
{
    alpha_ =
        math::clamp(static_cast<int>(math::clamp(alpha, 0.0f, 1.0f) * 255.0f), 0, 255);
}

void Color::setAlpha(int alpha) noexcept
{
    alpha_ = math::clamp(alpha, 0, 255);
}

float Color::getRed() const noexcept
{
    return static_cast<float>(red_) / 255.0f;
}

int Color::getRedInt() const noexcept
{
    return red_;
}

float Color::getGreen() const noexcept
{
    return static_cast<float>(green_) / 255.0f;
}

int Color::getGreenInt() const noexcept
{
    return green_;
}

float Color::getBlue() const noexcept
{
    return static_cast<float>(blue_) / 255.0f;
}

int Color::getBlueInt() const noexcept
{
    return blue_;
}

float Color::getAlpha() const noexcept
{
    return static_cast<float>(alpha_) / 255.0f;
}

int Color::getAlphaInt() const noexcept
{
    return alpha_;
}

bool Color::operator==(const Color& other) const noexcept
{
    return (red_ == other.red_ && green_ == other.green_ && blue_ == other.blue_ &&
            alpha_ == other.alpha_);
}

Color Color::operator-(const Color& rhs) const noexcept
{
    Color c;

    c.setRed(red_ - rhs.red_);
    c.setGreen(green_ - rhs.green_);
    c.setBlue(blue_ - rhs.blue_);
    c.setAlpha(alpha_ - rhs.alpha_);

    return c;
}

Color Color::operator+(const Color& rhs) const noexcept
{
    Color c;

    c.setRed(red_ + rhs.red_);
    c.setGreen(green_ + rhs.green_);
    c.setBlue(blue_ + rhs.blue_);
    c.setAlpha(alpha_ + rhs.alpha_);

    return c;
}
