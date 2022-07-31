#pragma once

#include <corgi/containers/Node.h>
#include <corgi/ecs/System.h>
#include <corgi/inputs/Inputs.h>
#include <corgi/ui/Widget.h>

#include <chrono>
#include <map>
#include <vector>

namespace corgi
{
    class Window;
    class Renderer;

    using NodeWidget = Node<std::unique_ptr<ui::Widget>>;

    struct ToolTip
    {
        std::string text;
        int          x;
        int          y;
    };

    class UISystem : public AbstractSystem
    {
    public:
        /**
         * @brief Construct a new UISystem object
         * 		  The UISystem needs a reference to the window so the root
         * 		  ui object are connected to the resize event. And a reference
         * 		  to the Inputs for updating the thing
         * @param window 
         * @param inputs 
         */
        UISystem(Renderer& renderer, const Inputs& inputs);

        void update(float elapsedTime) final;

        ui::Widget& emplace_back(Window& window);
        ui::Widget& addModalWidget(Window& window);

        /**
         * @brief   Checks if the mouse is close enough to the top edge
         * 
         *          Used to determine if a resizing event on the top border
         *          is happening
         */
        [[nodiscard]] bool checkTopEdge(const ui::Widget& widget) const noexcept;

        /**
         * @brief   Checks if the mouse is close enough to the bottom edge
         * 
         *          Used to determine if a resizing event on the bottom border
         *          is happening
         */
        [[nodiscard]] bool checkBottomEdge(const ui::Widget& widget) const noexcept;

        /**
         * @brief   Checks if the mouse is close enough to the left edge
         * 
         *          Used to determine if a resizing event on the left border
         *          is happening
         */
        [[nodiscard]] bool checkLeftEdge(const ui::Widget& widget) const noexcept;

        /**
         * @brief   Checks if the mouse is close enough to the right edge
         * 
         *          Used to determine if a resizing event on the right border
         *          is happening
         */
        [[nodiscard]] bool checkRightEdge(const ui::Widget& widget) const noexcept;

    protected:
        void handleMouseEnter(float elapsedTime, NodeWidget& node);
        void handleMouseExit(float elapsedTime, NodeWidget& nodeWidget);
        void handleMouseButtonDown(float elapsedTime, NodeWidget& nodeWidget);
        void handleMouseButtonUp(float elapsedTime, NodeWidget& nodeWidget);
        void handleWheelEvent(float elapsedTime, NodeWidget& nodeWidget);
        void handleMouseClick(float elapsedTime, NodeWidget& nodeWidget);

        /**
         * @brief Internally handles the mouse over event
         */
        void handle_mouse_over(float elapsedTime, NodeWidget& nodeWidget);

        /**
         * @brief Internally handles the mouse_drag_event, mouse_drag_start_event and mouse_drag_end_event
         */
        void handle_drag_event(float elapsed_time, NodeWidget& node_widget);

        /**
         * @brief   Resizes a Widget the user is currently editing
         */
        void resizeWidget(ui::Widget* widget);

        /**
         * @brief   Removes widgets that have been tagged for destruction
         */
        void removeDestroyedWidgets(NodeWidget& nodeWidget);

        void checkManualFocus(NodeWidget& nodeWidget);

        void checkEdges(ui::Widget& widget);

        bool cursor_updated_ = false;

        NodeWidget root_;
        NodeWidget modal_widgets_;
        NodeWidget tooltip_widgets_;

        std::vector<ui::Widget*>           resized_widgets_;
        std::map<ui::Widget*, ui::Widget*> toolTips_;

        float resize_start_x_ = 0.0f;
        float resize_start_y_ = 0.0f;

        float widget_resize_width_before_  = 0.0f;
        float widget_resize_height_before_ = 0.0f;

        std::vector<ui::Widget*> pushed_widgets_;

        /**
         * @brief   Search for widget marked for destruction
         * 
         *          Since a destroyed widget will destroy its children, we make
         *          sure not to add them to the list destroyed widget, otherwise
         *          we might delete the same widget twice, which will crash
         *          the application
         * 
         * @param widget current widget analysed
         * @param widgetsToDestroy& Reference to the vector that will holds the widgets to destroy
         */
        void searchForDestroyedWidget(ui::Widget*          widget,
                                      Vector<ui::Widget*>& widgetsToDestroy
            );

        /**
         * @brief Search for the FocusedWidget in one of the widget's child
         * 
         * @param widget 
         */
        void searchIfDestroyedWidgetWasFocused(ui::Widget* widget);

        /*!
         * @brief   Modal widgets will be displayed on top of everything
         *          and will stop events from propagating to the rest of the UI
         *          hierarchy
         */
        void startResizing(ui::Widget& widget);

        std::chrono::steady_clock::time_point previous_click_;

        bool left_mouse_button_pressed_ = false;

        float       time_since_last_button_down_ = 0.0f;
        ui::Widget* focused_widget_              = nullptr;
        /**
         * @brief   We keep a pointer to the widget currently being dragged
         * 
         *          For simplicity sake, there can be only 1 widget dragged at
         *          a time
         */
        ui::Widget*   dragged_widget_ = nullptr;
        const Inputs& inputs_;
        Renderer&     renderer_;
        float         edgeOffset_ = 6.0f;
    };
}    // namespace corgi
