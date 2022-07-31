#include "corgi/systems/UISystem.h"

#include <corgi/main/Game.h>
#include <corgi/main/Window.h>
#include <corgi/rendering/renderer.h>
#include <corgi/rendering/texture.h>
#include <corgi/systems/SpriteRendererSystem.h>
#include <corgi/ui/Rectangle.h>
#include <corgi/utils/ResourcesCache.h>

corgi::ui::Rectangle::Rectangle()
{
    mMaterial = *ResourcesCache::get<Material>(
        "corgi/materials/ui/uiRectangle.mat");
}

void corgi::ui::Rectangle::paint(Renderer& renderer)
{
    DrawList::Rectangle rectangle;

    if(mColorEasing)
    {
        if(mElapsedTimeEasing / mDuration >= 1.0f)
        {
            mColor       = mDestinationColor;
            mColorEasing = false;
        }
        else
        {
            float r =
                mStartColor.getRed() +
                (mDestinationColor.getRed() - mStartColor.getRed()) *
                easing::getEasing(mEasingEquation, mElapsedTimeEasing / mDuration);
            float g =
                mStartColor.getGreen() +
                (mDestinationColor.getGreen() - mStartColor.getGreen()) *
                easing::getEasing(mEasingEquation, mElapsedTimeEasing / mDuration);
            float b =
                mStartColor.getBlue() +
                (mDestinationColor.getBlue() - mStartColor.getBlue()) *
                easing::getEasing(mEasingEquation, mElapsedTimeEasing / mDuration);
            float a =
                mStartColor.getAlpha() +
                (mDestinationColor.getAlpha() - mStartColor.getAlpha()) *
                easing::getEasing(mEasingEquation, mElapsedTimeEasing / mDuration);

            mColor = Color(r, g, b, a);
        }
        mElapsedTimeEasing += 0.015f;
    }

    mMaterial.set_uniform("uMainColor", mColor);
    mMaterial.set_uniform("uRectangleWidth", width());
    mMaterial.set_uniform("uRectangleHeight", height());
    mMaterial.set_uniform("uRadius", mRadius);

    // TO BE FAIR ... that whole thing probably should be in widget
    // so I don't have to do it differently everytime
    renderer.window_draw_list().add_mesh(
        SpriteRendererSystem::getUiMesh(0),
        Matrix::translation(real_x(), real_y(), depth_) *
        Matrix::scale(width(), height(), 1.0f),
        mMaterial, *window_);
}

void corgi::ui::Rectangle::setColorEasing(const Color&           dest,
                                          easing::EasingEquation equation,
                                          float                  duration
    )
{
    mColorEasing       = true;
    mEasingEquation    = equation;
    mDuration          = duration;
    mDestinationColor  = dest;
    mElapsedTimeEasing = 0.0f;
    mStartColor        = mColor;
}

corgi::Color corgi::ui::Rectangle::color() const
{
    return mColor;
}

void corgi::ui::Rectangle::setColor(const corgi::Color& color)
{
    // We stop any color easing going on when calling SetColor
    mColorEasing = false;
    mColor       = color;
}

float corgi::ui::Rectangle::radius() const
{
    return mRadius;
}

void corgi::ui::Rectangle::setRadius(float radius)
{
    mRadius = radius;
}
