#include <corgi/ui/layouts/VerticalLayout.h>

void corgi::ui::VerticalLayout::setSpacing(float spacing)
{
    mSpacing = spacing;
}

float corgi::ui::VerticalLayout::getSpacing() const noexcept
{
    return mSpacing;
}

float corgi::ui::VerticalLayout::height() const noexcept
{
    float sum = 0.0f;

    switch(mStretchMode)
    {
        case StretchMode::StretchContentToFillLayout:
            return Widget::height();

        case StretchMode::StretchLayoutToFillContent:

            for(const auto& node : getChildren())
            {
                auto* widget = node.get()->mValue.get();
                sum += widget->height();
            }
            sum += (getChildren().size() + 1) * mSpacing;
            return sum;
    }
    return sum;
    return 0.0f;
}

void corgi::ui::VerticalLayout::update(float elapsed_time) {}

void corgi::ui::VerticalLayout::paint(Renderer& renderer)
{
    // Only usefull when in StretchContentToFillLayout mode
    float totalSpacing = (getChildren().size() + 1) * mSpacing;
    float usableHeight = height() - totalSpacing;
    float childHeight  = usableHeight / static_cast<float>(getChildren().size());

    Widget* w = nullptr;
    float   y = 0.0f;
    switch(mStretchMode)
    {
        case StretchMode::StretchContentToFillLayout:

            for(auto& child : getChildren())
            {
                y += mSpacing;
                w->y(y);
                w->setHeight(childHeight);
                y += childHeight;
            }
            break;

        case StretchMode::StretchLayoutToFillContent:

            for(auto& child : getChildren())
            {
                y += mSpacing;
                w = child.get()->mValue.get();
                w->y(y);
                y += w->height();
            }
            break;
    }
}
