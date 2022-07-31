#include <corgi/math/MathUtils.h>
#include <corgi/ui/Widget.h>

namespace corgi::ui
{

void Widget::setDimensions(float width,
                           float height,
                           bool  widthRelative,
                           bool  heightRelative)
{
    setWidth(width, widthRelative);
    setHeight(height, heightRelative);
}
void Widget::update(float elapsed_time) {}

void Widget::paint(Renderer& renderer) {}

Event<>& Widget::onEnable()
{
    return onEnable_;
}
Event<>& Widget::onDisable()
{
    return onDisable_;
}

void Widget::destroy()
{
    mIsDestroyed = true;
}

void Widget::remove(Widget* child)
{
    auto result =
        std::find_if(mNode->children_.begin(), mNode->children_.end(),
                     [&](const std::unique_ptr<Node<std::unique_ptr<Widget>>>& c)
                     { return c->mValue.get() == child; });

    if(result != mNode->children_.end())
    {
        child->mIsDestroyed = true;
    }
}

void Widget::setToolTip(const std::string& tooltip)
{
    if(tooltip == "")
    {
        hasTooltip_ = false;
        return;
    }

    hasTooltip_ = true;
    tooltip_    = tooltip;
}

bool Widget::getResizeTop() const noexcept
{
    return resize_.top;
}

bool Widget::getResizeLeft() const noexcept
{
    return resize_.left;
}

bool Widget::getResizeBottom() const noexcept
{
    return resize_.bottom;
}

void Widget::setResizeBottom(bool value, bool inPercentage)
{
    resize_.bottom             = value;
    resize_.bottomInPercentage = inPercentage;
}

void Widget::setRelativeRightBorder(float width, float offset)
{
    setLeft(100, true);
    setMarginLeft(-offset);
    setWidth(-(offset - width));
}

void Widget::alignVertical(Widget::VerticalAlign align)
{
    switch(align)
    {
        case Widget::VerticalAlign::Center:
            setTop((parent()->height() - height()) / 2.0f);
            break;

        case Widget::VerticalAlign::Top:
            setTop(0);
            break;

        case Widget::VerticalAlign::Bottom:
            setTop((parent()->height() - height()));
            break;
    }
}

const std::string& Widget::name() const noexcept
{
    return name_;
}

void Widget::set_name(const std::string& name)
{
    name_ = name;
}

Widget* Widget::find(const std::string& name) const noexcept
{
    for(const auto& child : *mNode)
    {
        if(child.mValue->name() == name)
        {
            return child.mValue.get();
        }
    }
    return nullptr;
}

void Widget::setResizeTop(bool value, bool inPercentage)
{
    resize_.top             = value;
    resize_.topInPercentage = inPercentage;
}

void Widget::setResizeLeft(bool value, bool inPercentage)
{
    resize_.left             = value;
    resize_.leftInPercentage = inPercentage;
}

void Widget::setResizeRight(bool value, bool inPercentage)
{
    resize_.right             = value;
    resize_.rightInPercentage = inPercentage;
}

bool Widget::getResizeRight() const noexcept
{
    return resize_.right;
}

bool Widget::contains(const int point_x, const int point_y) const
{
    const auto fpoint_x = static_cast<float>(point_x);
    const auto fpoint_y = static_cast<float>(point_y);

    // If we're not inside the closest parent viewport we just skip the current widget

    auto viewport     = findViewport();
    bool viewportTrue = false;

    if(fpoint_x >= viewport->real_x() &&
       fpoint_x <= (viewport->real_x() + viewport->width()))
        if(fpoint_y >= viewport->real_y() &&
           fpoint_y <= (viewport->real_y() + viewport->height()))
            viewportTrue = true;

    if(!viewportTrue)
        return false;

    if(fpoint_x >= real_x() && fpoint_x <= (real_x() + width()))
        return (fpoint_y >= real_y() && fpoint_y <= (real_y() + height()));
    return false;
}

void Widget::position(const float x, const float y, bool relative)
{
    x_ = x;
    y_ = y;

    x_relative_ = relative;
    y_relative_ = relative;

    updateDimension(this);
}

void Widget::dimension(const float w, const float h, bool relative)
{
    width_  = w;
    height_ = h;

    width_relative_  = relative;
    height_relative_ = relative;
    updateDimension(this);
}

void Widget::setWidth(const float val, bool relative)
{
    width_          = val;
    width_relative_ = relative;

    updateDimension(this);
}

void Widget::setHeight(const float val, bool relative)
{
    height_          = val;
    height_relative_ = relative;
    updateDimension(this);
}

void Widget::x(const float val, bool relative)
{
    x_          = val;
    x_relative_ = relative;

    updateDimension(this);
}

void Widget::setTop(const float val, const bool relative)
{
    y_          = val;
    y_relative_ = relative;
    updateDimension(this);
}

float Widget::getTop() const noexcept
{
    return y_;
}

void Widget::setLeft(const float val, const bool relative)
{
    x_          = val;
    x_relative_ = relative;

    updateDimension(this);
}

float Widget::getLeft() const noexcept
{
    return x_;
}

void Widget::y(const float val, bool relative)
{
    y_          = val;
    y_relative_ = relative;
    updateDimension(this);
}

void Widget::setFocusIn()
{
    mManualFocusIn = true;
}

void Widget::setFocusOut()
{
    mManualFocusOut = true;
}

void Widget::clearChildren()
{
    for(auto* child : getChildrenAsWidget())
        remove(child);
}

float Widget::width() const noexcept
{
    if(mRight)
    {
        float temp;
        switch(mRight->type)
        {
            case UnitType::Float:
                return parentWidthCache_ - mRight->value - x();
            case UnitType::Percentage:
                return parentWidthCache_ -
                       ((mRight->value / 100.0f) * parentWidthCache_) - x();
        }
    }

    float totalMargin = marginLeft() + marginRight();
    if(width_relative_)
        return parentWidthCache_ * width_ / 100.0f - totalMargin;

    return width_ - totalMargin;
}

void Widget::activate(bool rec)
{
    activity_ = true;
    onActivate_();
    if(rec)
    {
        for(auto child : getChildrenAsWidget())
        {
            child->activate(true);
        }
    }
}

bool Widget::isActive() const noexcept
{
    return activity_;
}

void Widget::deactivate(bool rec)
{
    activity_ = false;
    onDeactivate_();

    if(rec)
    {
        for(auto child : getChildrenAsWidget())
        {
            child->deactivate(true);
        }
    }
}

float Widget::height() const noexcept
{
    if(mBottom)
    {
        switch(mBottom->type)
        {
            case UnitType::Float:
                return parentHeightCache_ - mBottom->value - y();
            case UnitType::Percentage:
                return parentHeightCache_ -
                       (mBottom->value / 100.0f * parentHeightCache_) - y();
        }
    }

    float totalMargin = marginTop() + marginBottom();
    if(height_relative_)
        return parentHeightCache_ * height_ / 100.0f - totalMargin;

    return height_ - totalMargin;
}

enum class Axe
{
    Horizontal,
    Vertical
};

static float getValueFromUnit(Widget::Unit unit, const Widget* parent, Axe axe)
{
    switch(unit.type)
    {
        case Widget::UnitType::None:
            return 0.f;
        case Widget::UnitType::Float:
            return unit.value;
        case Widget::UnitType::Percentage:

            if(parent != nullptr)
            {
                switch(axe)
                {
                    case Axe::Horizontal:
                        return parent->width() * unit.value / 100.f;
                    case Axe::Vertical:
                        return parent->height() * unit.value / 100.f;
                }
            }
    }
    return 0.f;
}

void Widget::setAnchorsToFillParentSpace()
{
    x(0);
    y(0);
    setRight(0);
    setBottom(0);
}

bool Widget::isDestroyed() const noexcept
{
    return mIsDestroyed;
}

float Widget::marginLeft() const noexcept
{
    return getValueFromUnit(mMargin.left, parent(), Axe::Horizontal);
}

float Widget::marginRight() const
{
    return getValueFromUnit(mMargin.right, parent(), Axe::Horizontal);
}

float Widget::marginTop() const
{
    return getValueFromUnit(mMargin.top, parent(), Axe::Vertical);
}

float Widget::marginBottom() const
{
    return getValueFromUnit(mMargin.down, parent(), Axe::Vertical);
}

void Widget::setMarginLeft(float value, bool isPercentage)
{
    mMargin.left.value = value;
    mMargin.left.type  = isPercentage ? UnitType::Percentage : UnitType::Float;

    updateDimension(this);
}

void Widget::setMarginBottom(float value, bool isPercentage)
{
    mMargin.down.value = value;
    mMargin.down.type  = isPercentage ? UnitType::Percentage : UnitType::Float;
    updateDimension(this);
}

void Widget::setMarginTop(float value, bool isPercentage)
{
    mMargin.top.value = value;
    mMargin.top.type  = isPercentage ? UnitType::Percentage : UnitType::Float;
    updateDimension(this);
}

void Widget::setMarginRight(float value, bool isPercentage)
{
    mMargin.right.value = value;
    mMargin.right.type  = isPercentage ? UnitType::Percentage : UnitType::Float;

    updateDimension(this);
}

void Widget::setRight(float value, bool isRelative)
{
    mRight = {value, isRelative ? UnitType::Percentage : UnitType::Float};

    updateDimension(this);
}

void Widget::setBottom(float value, bool isRelative)
{
    mBottom = {value, isRelative ? UnitType::Percentage : UnitType::Float};
    updateDimension(this);
}

float Widget::x() const noexcept
{
    if(x_relative_)
        return marginLeft() + parentWidthCache_ * x_ / 100.0f;
    return marginLeft() + x_;
}

float Widget::y() const noexcept
{
    if(y_relative_)
        return parentHeightCache_ * y_ / 100.0f + marginTop();
    return y_ + marginTop();
}

float Widget::real_x() const
{
    return parentX_ + x();
}

float Widget::real_y() const
{
    return parentY_ + y();
}

void Widget::actual_paint(Renderer& renderer)
{
    if(!is_enabled_ || mIsDestroyed)
        return;

    paint(renderer);

    for(auto& child : getChildren())
        child->mValue->actual_paint(renderer);
}

Widget::Children& Widget::getChildren()
{
    return mNode->children_;
}

const Widget::Children& Widget::getChildren() const
{
    return mNode->children_;
}

std::vector<Widget*> Widget::getChildrenAsWidget() const
{
    std::vector<Widget*> children;
    children.reserve(mNode->children_.size());

    /* transform(mNode->children_.begin(), mNode->children_.end(), children.begin(),
                  [&](std::unique_ptr<Node<std::unique_ptr<Widget>>>& child)
                  {
                      return child->mValue.get();
                  });*/

    for(auto& child : mNode->children_)
    {
        children.push_back(child->mValue.get());
    }

    return children;
}

Widget* Widget::parent()
{
    if(mNode->parent_)
        return mNode->parent_->mValue.get();
    return nullptr;
}

const Widget* Widget::parent() const
{
    if(mNode->parent_)
        return mNode->parent_->mValue.get();
    return nullptr;
}

bool Widget::resizeEventTriggered() const
{
    return triggerResizeEvent_;
}

UISystem* Widget::uiSystem()
{
    return uiSystem_;
}

void Widget::checkForResizeEvent()
{
    const auto w = width();
    const auto h = height();

    triggerResizeEvent_ = false;

    if(mPreviousWidth != w || mPreviousHeight != h)
    {
        triggerResizeEvent_ = true;
        mResizeEvent(w, h);
    }

    mPreviousWidth  = w;
    mPreviousHeight = h;
}

void Widget::updateDimension(Widget* ww)
{
    for(auto& child : ww->mNode->children_)
    {
        child.get()->mValue.get()->parentX_           = ww->real_x();
        child.get()->mValue.get()->parentWidthCache_  = ww->width();
        child.get()->mValue.get()->parentHeightCache_ = ww->height();
        child.get()->mValue.get()->parentY_           = ww->real_y();

        updateDimension(child.get()->mValue.get());
    }
}

bool Widget::isEnabled() const noexcept
{
    return is_enabled_;
}

void Widget::disable() noexcept
{
    is_enabled_ = false;
    onDisable_();
    for(auto& child : mNode->children_)
    {
        child.get()->mValue->disable();
    }
}

void Widget::enable(bool rec) noexcept
{
    is_enabled_ = true;
    onEnable_();
    if(rec)
    {
        for(auto& child : mNode->children_)
            child.get()->mValue->enable(true);
    }
}

const Widget* Widget::findViewport() const
{
    if(isViewport)
        return this;

    if(mNode->parent_)
        return mNode->parent_->mValue->findViewport();

    return nullptr;
}
}    // namespace corgi::ui
