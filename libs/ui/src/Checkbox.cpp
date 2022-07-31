#include "corgi/systems/UISystem.h"

#include <corgi/main/Cursor.h>
#include <corgi/main/Window.h>
#include <corgi/rendering/texture.h>
#include <corgi/ui/Checkbox.h>
#include <corgi/ui/Image.h>
#include <corgi/utils/ResourcesCache.h>
#include <corgi/main/Game.h>

using namespace corgi::ui;

Checkbox::Checkbox()
{}

void Checkbox::init()
{
    mImage = &emplace_back<Image>();

    mImage->x(0);
    mImage->y(0);
    mImage->setRight(0);
    mImage->setBottom(0);

    mOnMouseClick += [&](int value, int val)
    {
        mIsChecked = !mIsChecked;
        if(mIsChecked == true)
        {
            if(isIndeterminated())
            {
                mIsIndeterminated = false;
            }
        }
        mOnChange(mIsChecked);
        mPropagateEvent = false;
    };

    onMouseEnter_ += [&](int a, int b)
    {
        Cursor::set(Cursor::DefaultCursor::Hand);
    };

    onMouseExit_ += [&](int a, int b)
    {
        Cursor::set(Cursor::DefaultCursor::Arrow);
    };

    // Default width/height
    setWidth(20.0f);
    setHeight(20.0f);
}

void Checkbox::paint(Renderer& renderer)
{
    if(mIsIndeterminated)
    {
        mImage->set_image(*ResourcesCache::get<Texture>(
            "corgi/textures/CheckboxIndeterminate.tex"));
        return;
    }
    if(mIsChecked)
    {
        mImage->set_image(*ResourcesCache::get<Texture>(
            "corgi/textures/CheckboxChecked.tex"));
    }
    else
    {
        mImage->set_image(*ResourcesCache::get<Texture>(
            "corgi/textures/CheckboxUnChecked.tex"));
    }
}

corgi::Event<bool>& Checkbox::onChanged()
{
    return mOnChange;
}

bool Checkbox::isChecked() const
{
    return mIsChecked;
}

void Checkbox::check()
{
    mIsChecked = true;
}

void Checkbox::uncheck()
{
    mIsChecked = false;
}

bool Checkbox::isIndeterminated()
{
    return mIsIndeterminated;
}

void Checkbox::setIndeterminate(bool value)
{
    mIsIndeterminated = value;
}
