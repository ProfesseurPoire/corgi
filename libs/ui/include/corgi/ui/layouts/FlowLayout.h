#pragma once

#include <corgi/ui/Widget.h>

namespace corgi::ui
{
    /**
     * @brief The FlowLayout class order items in a dynamic grid depending
     *        on the item's size and the container's size
     */
    class FlowLayout : public Widget
    {
    public:
        /**
         * @brief Defines in which direction the layout should get extended 
         */
        enum class FlowDirection
        {
            Down,
            Right
        };

        /**
         * @brief Defined how child elements should be wrapped
         */
        enum class Wrap
        {
            Row,
            Column
        };

        // Probably something I should hide in protected no?
        void paint(Renderer& renderer) override;

        /**
         * @brief Set the dimensions of items managed by the layout
         * 
         * @param itemWidth 
         * @param itemHeight 
         */
        void setItemDimension(float itemWidth, float itemHeight);

        void setSpacing(float spacing);
        void setItemWidth(float itemWidth);
        void setItemHeight(float itemHeight);
        void setFlowDirection(FlowDirection flowDirection);
        void setWrapMode(Wrap wrapMode);

        [[nodiscard]] float         getSpacing() const;
        [[nodiscard]] float         getItemWidth() const;
        [[nodiscard]] float         getItemHeight() const;
        [[nodiscard]] FlowDirection getFlowDirection() const;
        [[nodiscard]] Wrap          getWrapMode() const;

        float width() const noexcept override;
        float height() const noexcept override;

        /*!
         * @brief   Gets the column count
         *          This function calculates how many columns are displayed
         * @return  Returns the column count as an int
         */
        [[nodiscard]] int getCountColumns() const noexcept;

        /*!
         * @brief   Gets the row count
         *          This function calculates how many rows are displayed using
         * @return  Returns an int with the column count
         */
        [[nodiscard]] int getCountRows() const noexcept;

        void handleNewChild(Widget* w)
        {
            w->setWidth(mItemWidth);
            w->setHeight(mItemHeight);
        }

    private:
        /**
         * @brief Width of an item inside the layout
         */
        float mItemWidth {100.f};

        /**
         * @brief Height of an item inside the layout
         */
        float mItemHeight {100.f};

        /**
         * @brief Space between items inside the FlowLayout
         */
        float mSpacing {0.f};

        FlowDirection mFlowDirection = FlowDirection::Down;
        Wrap          mWrapMode      = Wrap::Row;
    };
}    // namespace corgi::ui
