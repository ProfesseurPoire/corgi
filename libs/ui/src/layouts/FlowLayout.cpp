#include "corgi/utils/ResourcesCache.h"

#include <corgi/ui/layouts/FlowLayout.h>
#include <math.h>

using namespace corgi::ui;

float FlowLayout::width() const noexcept
{
    switch(mFlowDirection)
    {
        case FlowDirection::Down:
            return Widget::width();

        // FlowDirection::Right means we expand the layout on the x axis
        // depending on the items size, spacing, and WrapMode
        case FlowDirection::Right:
            return getCountColumns() * mItemWidth + (getCountColumns() + 1) * mSpacing;
    }
    return 0.0f;
}

float FlowLayout::height() const noexcept
{
    switch(mFlowDirection)
    {
        // FlowDirection::Down means we expend the layout on the y axis
        case FlowDirection::Down:
            return getCountRows() * mItemHeight + (getCountRows() + 1) * mSpacing;
        case FlowDirection::Right:
            return Widget::height();
    }
    return 0.0f;
}

void FlowLayout::setSpacing(float spacing)
{
    mSpacing = spacing;
}

void FlowLayout::setItemWidth(float itemWidth)
{
    mItemWidth = itemWidth;
}

void FlowLayout::setItemDimension(float itemWidth, float itemHeight)
{
    mItemWidth  = itemWidth;
    mItemHeight = itemHeight;
}

void FlowLayout::setItemHeight(float itemHeight)
{
    mItemHeight = itemHeight;
}

float FlowLayout::getSpacing() const
{
    return mSpacing;
}

float FlowLayout::getItemWidth() const
{
    return mItemWidth;
}

FlowLayout::FlowDirection FlowLayout::getFlowDirection() const
{
    return mFlowDirection;
}

FlowLayout::Wrap FlowLayout::getWrapMode() const
{
    return mWrapMode;
}

void FlowLayout::setFlowDirection(FlowLayout::FlowDirection direction)
{
    mFlowDirection = direction;
}

void FlowLayout::setWrapMode(FlowLayout::Wrap wrapMode)
{
    mWrapMode = wrapMode;
}

float FlowLayout::getItemHeight() const
{
    return mItemHeight;
}

int FlowLayout::getCountColumns() const noexcept
{
    int columnCount = 0;

    switch(mWrapMode)
    {
        case FlowLayout::Wrap::Row:

            if(mFlowDirection == FlowLayout::FlowDirection::Right)
                return getChildren().size();

            // using a loop here, though it's not really performant, at least it works
            // well and most of the time we won't have a lot of columns
            // we could also do that only after a resize event or something later on
            // so we don't do it every paint event
            while((columnCount * mItemWidth + mSpacing * (columnCount + 1) < width()))
            {
                columnCount++;
            }
            columnCount--;

            if(columnCount == 0)
                columnCount = 1;
            return columnCount;

        case FlowLayout::Wrap::Column:

            if(mFlowDirection == FlowLayout::FlowDirection::Down)
                return 1;
            return getChildren().size() / getCountRows();
            break;
    }
    return 0;
}

int FlowLayout::getCountRows() const noexcept
{
    int rowCount = 0;

    switch(mWrapMode)
    {
        case Wrap::Row:
            if(mFlowDirection == FlowDirection::Right)
                return 1;
            return getChildren().size() / getCountColumns();

        case Wrap::Column:

            if(mFlowDirection == FlowLayout::FlowDirection::Down)
                return getChildren().size();

            while((rowCount * mItemHeight + mSpacing * (rowCount + 1) < height()))
            {
                rowCount++;
            }
            rowCount--;

            if(rowCount == 0)
                rowCount = 1;
            return rowCount;
    }
    // TODO : I know it's to efficient to compute columns count but is ok for now
    return getChildren().size() / getCountColumns();
}

void FlowLayout::paint(Renderer& renderer)
{
    int columns = getCountColumns();
    int rows    = getCountRows();

    int row;
    int column;
    int i = 0;

    Widget* widget;

    switch(mWrapMode)
    {
        case FlowLayout::Wrap::Column:

            for(auto& child : getChildren())
            {
                auto column = floorf(i / rows);
                auto row    = i % rows;

                auto& widget = dynamic_cast<Widget&>(*child.get()->mValue.get());

                widget.x(column * mItemWidth + mSpacing + column * mSpacing);
                widget.y(row * mItemHeight + mSpacing + row * mSpacing);

                widget.setHeight(mItemHeight);
                widget.setWidth(mItemWidth);

                i++;
            }

            break;

        case FlowLayout::Wrap::Row:
            for(auto& child : getChildren())
            {
                auto column = i % columns;
                auto row    = floorf(i / columns);

                auto unusedWidth =
                    width() - columns * getItemWidth() - (1.0f * getSpacing());
                float realSpacing = unusedWidth / (columns);

                auto& widget = dynamic_cast<Widget&>(*child.get()->mValue.get());

                widget.x(column * mItemWidth + mSpacing + column * realSpacing);
                widget.y(row * mItemHeight + mSpacing + row * mSpacing);

                widget.setHeight(mItemHeight);
                widget.setWidth(mItemWidth);

                i++;
            }

            break;
    }
}