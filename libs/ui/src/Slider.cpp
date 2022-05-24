#include <corgi/main/Cursor.h>
#include <corgi/math/MathUtils.h>
#include <corgi/systems/SpriteRendererSystem.h>
#include <corgi/ui/Slider.h>
#include <corgi/utils/ResourcesCache.h>

using namespace corgi::ui;

void Slider::setCurrentValue(float value)
{
    if(value == mCurrentValue)
        return;

    mCurrentValue = corgi::math::clamp(value, mMinValue, mMaxValue);
    mOnValueChanged(getCurrentValue());
}

void Slider::setMinValue(float value)
{
    mMinValue     = value;
    mCurrentValue = corgi::math::max(mCurrentValue, mMinValue);
}

void Slider::setMaxValue(float value)
{
    mMaxValue     = value;
    mCurrentValue = corgi::math::min(mCurrentValue, mMaxValue);
}

Slider::Orientation Slider::getOrientation() const
{
    return mOrientation;
}

void Slider::setOrientation(Slider::Orientation orientation)
{
    mOrientation = orientation;
}

corgi::Event<float>& Slider::onValueChanged()
{
    return mOnValueChanged;
}

Slider::Slider() {}

void Slider::init()
{
    mMaterial  = *ResourcesCache::get<Material>("corgi/materials/ui/uiRectangle.mat");
    mContainer = &emplace_back<Rectangle>();
    mHandle    = &emplace_back<Rectangle>();

    mHandle->onMouseEnter() += [&](int x, int y)
    {
        Cursor::set(Cursor::DefaultCursor::Hand);
        mHandle->setColor(Color(0.8f, 0.8f, 0.9f, 1.0f));
    };

    mHandle->onMouseExit() += [&](int x, int y)
    {
        Cursor::set(Cursor::DefaultCursor::Arrow);
        mHandle->setColor(Color(0.4f, 0.4f, 0.5f, 1.0f));
    };

    mHandle->on_mouse_drag_begin() += [&](int x, int y)
    {
        switch(mOrientation)
        {
            case Orientation::Horizontal:
                mDragOffset = x - mHandle->real_x();
                break;
            case Orientation::Vertical:
                mDragOffset = y - mHandle->real_y();
                break;
        }
        mHandle->setColor(Color(0.2f, 0.2f, 0.30f, 1.0f));
    };

    mHandle->on_mouse_drag_end() += [&](int x, int y)
    {
        mHandle->setColor(Color(0.4f, 0.4f, 0.5f, 1.0f));
    };

    mHandle->on_mouse_drag() += [&](int x, int y)
    {
        auto previousValue = mCurrentValue;

        switch(mOrientation)
        {
            case Orientation::Horizontal:
                mCurrentValue =
                    corgi::math::clamp((((float(x) - mDragOffset + mHandle->width() / 2.0f) - mContainer->real_x()) /
                                        mContainer->width()) *
                                               (mMaxValue - mMinValue) +
                                           mMinValue,
                                       mMinValue, mMaxValue);
                break;
            case Orientation::Vertical:

                mCurrentValue =
                    corgi::math::clamp((((float(y) - mDragOffset + mHandle->height() / 2.0f) - mContainer->real_y()) /
                                        mContainer->height()) *
                                               (mMaxValue - mMinValue) +
                                           mMinValue,
                                       mMinValue, mMaxValue);
                break;
        }

        if(previousValue != mCurrentValue)
            mOnValueChanged(mCurrentValue);
    };

    // mContainer->x(0);
    // mContainer->setRight(0);

    // float containerHeight = 20;

    // mContainer->y(height()/2.f- containerHeight/2.0f);
    // mContainer->height(20);
    // mContainer->setColor(0.85f,0.85f,0.9f,1.0f);
    // mContainer->setRadius(5.0f);

    // mHandle->width(20);
    // mHandle->setRadius(10.0f);
    // mHandle->setColor(0.4f, 0.4f, 0.5f, 1.0f);
    // mHandle->height(40.0f);
    mHandle->setRadius(10.0f);
}

void Slider::paint(Renderer& SpriteRendererSystem)
{
    float containerHeight = 20.0f;
    float offset          = 0.0f;

    mContainer->setColor(Color(0.7f, 0.7f, 0.8f, 1.0f));
    mContainer->setRadius(0.0f);

    switch(mOrientation)
    {
        case Orientation::Horizontal:

            mContainer->x(0);
            mContainer->setRight(0);

            mContainer->y((height() / 2.f) - (containerHeight / 2.0f));
            mContainer->setHeight(20);

            mHandle->y((height() / 2.f) - (mHandle->height() / 2.0f));
            mHandle->setHeight(mContainerSize + 20.0f);
            mHandle->setWidth(20);

            offset = (mCurrentValue - mMinValue) / (mMaxValue - mMinValue) * mContainer->width();
            mHandle->x(offset - mHandle->width() / 2.0f);

            break;

        case Orientation::Vertical:

            mContainer->y(0);
            mContainer->setBottom(0);

            mContainer->x((width() / 2.f) - (mContainerSize / 2.0f));
            mContainer->setWidth(mContainerSize);

            mHandle->setHeight(20.0f);
            mHandle->setWidth(mContainerSize + 20.0f);

            mHandle->x((width() / 2.f) - (mHandle->width() / 2.0f));
            offset = (mCurrentValue - mMinValue) / (mMaxValue - mMinValue) * mContainer->height();
            mHandle->y(offset - mHandle->height() / 2.0f);
            break;

            break;
    }

    /*for(auto& child : children_)
        dynamic_cast<Widget*>(child.get())->paint(renderer,cache);*/
}
