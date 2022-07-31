#include <corgi/main/Cursor.h>
#include <corgi/main/Window.h>
#include <corgi/math/MathUtils.h>
#include <corgi/rendering/renderer.h>
#include <corgi/systems/UISystem.h>
#include <corgi/ui/Rectangle.h>
#include <corgi/ui/Text.h>
#include <corgi/utils/ResourcesCache.h>

using namespace corgi;

UISystem::UISystem(Renderer& renderer, const Inputs& inputs)
    : inputs_(inputs)
    , renderer_(renderer)
{
    root_.mValue.reset(new ui::Widget());
    root_.mValue->mNode      = &root_;
    root_.mValue->isViewport = true;
    root_.mValue->uiSystem_  = this;

    //mRoot->get()->mViewport = &mRoot;

    modal_widgets_.mValue.reset(new ui::Widget());
    modal_widgets_.mValue->mNode      = &modal_widgets_;
    modal_widgets_.mValue->isViewport = true;
    modal_widgets_.mValue->uiSystem_  = this;

    tooltip_widgets_.mValue.reset(new ui::Widget());
    tooltip_widgets_.mValue->mNode      = &tooltip_widgets_;
    tooltip_widgets_.mValue->isViewport = true;
    tooltip_widgets_.mValue->uiSystem_  = this;

    //mRoot->get()->mViewport = &mRoot;
}

static ui::Widget* findDeepestWidget(const Mouse&                       mouse,
                                     Node<std::unique_ptr<ui::Widget>>& root)
{
    ui::Widget* selectedWidget = nullptr;

    int depth = -1;

    for(auto& node : root)
    {
        auto* widget = node.mValue.get();
        auto  d      = node.depth();

        // We skip the widget if we manually disable its events
        if(widget->eventsDisabled())
            continue;

        // We skip a widget if it's disabled
        if(!widget->isEnabled())
            continue;

        // We skip a deactivated widget
        if(!widget->isActive())
            continue;

        if(widget->contains(mouse.x(), mouse.y()) && d > depth)
        {
            if(widget->mProcessEvent)
            {
                selectedWidget = widget;
                depth          = node.depth();
            }
        }
    }
    return selectedWidget;
}

void UISystem::handleMouseButtonDown(const float elapsedTime, NodeWidget& nodeWidget)
{
    const auto& mouse = inputs_.mouse();
    time_since_last_button_down_ += elapsedTime;
    // We're doing Event Bubbling. So we first look for the innermost widget
    // and then propagate the event to its parents
    if(mouse.is_button_down(Mouse::Button::Left))
    {
        auto widget = findDeepestWidget(mouse, nodeWidget);

        if(widget != nullptr)
        {
            do
            {
                //widget->mPropagateEvent = true;
                widget->mouseDownEvent_(mouse);
                pushed_widgets_.push_back(widget);
                widget->mPropagateEvent ? widget = widget->parent() : widget = nullptr;
            } while(widget);

            time_since_last_button_down_ = 0.0f;
        }
    }
}

void UISystem::handle_mouse_over(float elapsed_time, NodeWidget& node_widget)
{
    if(inputs_.mouse().has_moved())
    {
        auto widget = findDeepestWidget(inputs_.mouse(), node_widget);

        if(!widget)
            return;

        if(widget->contains(inputs_.mouse().x(), inputs_.mouse().y()))
        {
            widget->on_mouse_over_(inputs_.mouse());
        }
    }
}

void UISystem::handle_drag_event(float elapsed_time, NodeWidget& node_widget)
{
    // If there's no widget currently being dragged
    if(dragged_widget_ == nullptr)
    {
        // We check if we pressed the left mouse button
        if(inputs_.mouse().is_button_down(Mouse::Button::Left))
        {
            // We try to find the deepest widget
            dragged_widget_ = findDeepestWidget(inputs_.mouse(), node_widget);

            // If we found a widget, we trigger the mouse_drag_start_event
            if(dragged_widget_)
                dragged_widget_->mouse_drag_start_event_(inputs_.mouse());
        }
        return;
    }
    else
    {
        // If we're currently dragging a widget but we detect that
        // we released the left mouse button, we trigger
        // the mouse_drag_end_event
        if(inputs_.mouse().is_button_up(Mouse::Button::Left))
        {
            dragged_widget_->mouse_drag_end_event_(inputs_.mouse());
            dragged_widget_ = nullptr;
            return;
        }

        if(inputs_.mouse().has_moved())
            dragged_widget_->mouse_drag_event_(inputs_.mouse());
    }
}

void UISystem::handleMouseClick(float elapsedTime, NodeWidget& nodeWidget)
{
    const auto& mouse = inputs_.mouse();

    if(mouse.is_button_up(Mouse::Button::Left))
    {
        auto        widget         = findDeepestWidget(mouse, nodeWidget);
        ui::Widget* previousWidget = nullptr;

        if(widget)
        {
            auto now = std::chrono::steady_clock::now();
            auto ms  = std::chrono::duration_cast<std::chrono::milliseconds>(
                          now - previous_click_)
                          .count();

            do
            {
                previousWidget = widget;
                auto it =
                    std::find(pushed_widgets_.begin(), pushed_widgets_.end(), widget);

                if(it != pushed_widgets_.end())
                {
                    //widget->mPropagateEvent = true;
                    widget->mOnMouseClick(mouse.x(), mouse.y());

                    if(ms < 500)
                    {
                        widget->mMouseDoubleClickEvent(mouse.x(), mouse.y());
                    }

                    widget->mPropagateEvent ? widget = widget->parent() :
                                              widget = nullptr;
                }
                else
                {
                    widget = nullptr;
                }
            } while(widget);

            previous_click_ = now;

            // The last propagated widget will trigger the focus event
            // (if there's a change of course)
            if(focused_widget_ != nullptr)
            {
                if(focused_widget_ != previousWidget)
                {
                    focused_widget_->mFocusOutEvent();
                    focused_widget_->mFocus = false;
                }
            }

            if(focused_widget_ != previousWidget)
            {
                previousWidget->mFocusInEvent();
                focused_widget_        = previousWidget;
                previousWidget->mFocus = true;
            }
        }

        pushed_widgets_.clear();
    }
}

void UISystem::searchIfDestroyedWidgetWasFocused(ui::Widget* widget)
{
    if(focused_widget_ == widget)
    {
        focused_widget_ = nullptr;
        return;
    }

    for(auto child : widget->getChildrenAsWidget())
        searchIfDestroyedWidgetWasFocused(child);
}

void UISystem::searchForDestroyedWidget(ui::Widget*          widget,
                                        Vector<ui::Widget*>& widgetToDestroy)
{
    // If the current widget is marked for destruction
    // we add it to the list, and we don't scan its children since
    // they will be deleted anyways
    if(widget->isDestroyed())
    {
        widgetToDestroy.push_back(widget);
        return;
    }

    for(auto* child : widget->getChildrenAsWidget())
        searchForDestroyedWidget(child, widgetToDestroy);
}

void UISystem::removeDestroyedWidgets(NodeWidget& nodeWidget)
{
    Vector<ui::Widget*> widgets;
    searchForDestroyedWidget(nodeWidget.mValue.get(), widgets);

    // We loop through the widget marked for destruction
    for(auto* widget : widgets)
    {
        searchIfDestroyedWidgetWasFocused(widget);

        if(widget->parent())
        {
            widget->parent()->mNode->children_.erase(std::find_if(
                widget->parent()->mNode->children_.begin(),
                widget->parent()->mNode->children_.end(),
                [&](const std::unique_ptr<Node<std::unique_ptr<ui::Widget>>>& w)
                { return w->mValue.get() == widget; }));
        }
        else
        {
            // We actually don't want to destroy the root widgets (modal/tooltips or root)
        }
    }
}

void UISystem::checkManualFocus(NodeWidget& nodeWidget)
{
    ui::Widget* focusIn = nullptr;

    for(auto& node : nodeWidget)
    {
        if(node.mValue->mManualFocusIn)
        {
            focusIn                     = node.mValue.get();
            node.mValue->mManualFocusIn = false;
        }
    }

    ui::Widget* focusOut = nullptr;

    for(auto& node : nodeWidget)
    {
        if(node.mValue->mManualFocusOut)
        {
            focusOut                     = node.mValue.get();
            node.mValue->mManualFocusOut = false;
        }
    }

    if(focusIn)
    {
        if(focused_widget_)
        {
            focused_widget_->mFocusOutEvent();
            focused_widget_->mFocus = false;
        }

        focusIn->mFocusInEvent();
        focused_widget_         = focusIn;
        focused_widget_->mFocus = true;
    }

    if(focusOut)
    {
        focusOut->mFocusOutEvent();
        focusOut->mFocus = false;
    }
}

bool UISystem::checkRightEdge(const ui::Widget& widget) const noexcept
{
    if(!widget.resize_.right)
        return false;

    return math::in_range(static_cast<float>(inputs_.mouse().x()),
                          widget.real_x() + widget.width(),
                          widget.real_x() + widget.width() + edgeOffset_) &&
           math::in_range(static_cast<float>(inputs_.mouse().y()), widget.real_y(),
                          widget.real_y() + widget.height());
}

bool UISystem::checkLeftEdge(const ui::Widget& widget) const noexcept
{
    if(!widget.resize_.left)
        return false;

    return math::in_range(static_cast<float>(inputs_.mouse().x()),
                          widget.real_x() - edgeOffset_, widget.real_x()) &&
           math::in_range(static_cast<float>(inputs_.mouse().y()),
                          widget.real_y() - edgeOffset_,
                          widget.real_y() + widget.height() + edgeOffset_);
}

bool UISystem::checkTopEdge(const ui::Widget& widget) const noexcept
{
    if(!widget.resize_.top)
        return false;

    return math::in_range(static_cast<float>(inputs_.mouse().x()),
                          widget.real_x() - edgeOffset_,
                          widget.real_x() + widget.width() + edgeOffset_) &&
           math::in_range(static_cast<float>(inputs_.mouse().y()),
                          widget.real_y() - edgeOffset_, widget.real_y());
}

bool UISystem::checkBottomEdge(const ui::Widget& widget) const noexcept
{
    if(!widget.resize_.bottom)
        return false;

    return math::in_range(static_cast<float>(inputs_.mouse().x()), widget.real_x(),
                          widget.real_x() + widget.width()) &&
           math::in_range(static_cast<float>(inputs_.mouse().y()),
                          widget.real_y() + widget.height() - edgeOffset_,
                          widget.real_y() + widget.height() + edgeOffset_);
}

void UISystem::checkEdges(ui::Widget& widget)
{
    const bool left   = checkLeftEdge(widget);
    const bool right  = checkRightEdge(widget);
    const bool top    = checkTopEdge(widget);
    const bool bottom = checkBottomEdge(widget);

    if(left && !right && !top && !bottom)    // Left Only
    {
        if(inputs_.mouse().is_button_down(Mouse::Button::Left))
        {
            widget.currentResize_ = corgi::ui::Widget::ResizeEnum::W;
            startResizing(widget);
        }
        cursor_updated_ = true;
        return Cursor::set(Cursor::DefaultCursor::SizeWE);
    }

    if(!left && right && !top && !bottom)    // Right edge
    {
        if(inputs_.mouse().is_button_down(Mouse::Button::Left))
        {
            widget.currentResize_ = corgi::ui::Widget::ResizeEnum::E;
            startResizing(widget);
        }
        cursor_updated_ = true;
        return Cursor::set(Cursor::DefaultCursor::SizeWE);
    }

    if(left && !right && top && !bottom)    // Top left corner
    {
        if(inputs_.mouse().is_button_down(Mouse::Button::Left))
        {
            widget.currentResize_ = corgi::ui::Widget::ResizeEnum::NW;
            startResizing(widget);
        }
        cursor_updated_ = true;
        return Cursor::set(Cursor::DefaultCursor::SizeNWSE);
    }

    if(left && !right && !top && bottom)    // Bottom left corner
    {
        if(inputs_.mouse().is_button_down(Mouse::Button::Left))
        {
            widget.currentResize_ = corgi::ui::Widget::ResizeEnum::SW;
            startResizing(widget);
        }
        cursor_updated_ = true;
        return Cursor::set(Cursor::DefaultCursor::SizeNESW);
    }

    if(!left && !right && !top && bottom)    //Bottom edge
    {
        if(inputs_.mouse().is_button_down(Mouse::Button::Left))
        {
            widget.currentResize_ = corgi::ui::Widget::ResizeEnum::S;
            startResizing(widget);
        }
        cursor_updated_ = true;
        return Cursor::set(Cursor::DefaultCursor::SizeNS);
    }

    if(!left && !right && top && !bottom)    // Top edge
    {
        if(inputs_.mouse().is_button_down(Mouse::Button::Left))
        {
            widget.currentResize_ = corgi::ui::Widget::ResizeEnum::N;
            startResizing(widget);
        }
        cursor_updated_ = true;
        return Cursor::set(Cursor::DefaultCursor::SizeNS);
    }

    if(!left && right && top && !bottom)    // Top Right corner
    {
        if(inputs_.mouse().is_button_down(Mouse::Button::Left))
        {
            widget.currentResize_ = corgi::ui::Widget::ResizeEnum::NE;
            startResizing(widget);
        }
        cursor_updated_ = true;
        return Cursor::set(Cursor::DefaultCursor::SizeNESW);
    }

    if(!left && right && !top && bottom)    // Bottom right corner
    {
        if(inputs_.mouse().is_button_down(Mouse::Button::Left))
        {
            widget.currentResize_ = corgi::ui::Widget::ResizeEnum::SE;
            startResizing(widget);
        }
        cursor_updated_ = true;
        return Cursor::set(Cursor::DefaultCursor::SizeNWSE);
    }
}

void UISystem::startResizing(ui::Widget& widget)
{
    resized_widgets_.push_back(&widget);
    resize_start_x_ = static_cast<float>(inputs_.mouse().x());
    resize_start_y_ = static_cast<float>(inputs_.mouse().y());

    widget_resize_height_before_ = widget.height();
    widget_resize_width_before_  = widget.width();
}

void UISystem::handleMouseButtonUp(float elapsedTime, NodeWidget& nodeWidget)
{
    const auto& mouse = inputs_.mouse();
    // We're doing Event Bubbling. So we first look for the innermost widget
    // and then propagate the event to its parents
    if(mouse.is_button_up(Mouse::Button::Left))
    {
        auto widget = findDeepestWidget(mouse, nodeWidget);

        if(widget)
        {
            do
            {
                //widget->mPropagateEvent = true;
                widget->mouseUpEvent_(mouse);
                widget->mPropagateEvent ? widget = widget->parent() : widget = nullptr;
            } while(widget);
        }
    }
}

void UISystem::handleWheelEvent(float elapsedTime, NodeWidget& nodeWidget)
{
    const auto& mouse = inputs_.mouse();

    // We're doing Event Bubbling. So we first look for the innermost widget
    // and then propagate the event to its parents
    if(mouse.wheelDelta() != 0)
    {
        auto widget = findDeepestWidget(mouse, nodeWidget);

        if(widget)
        {
            do
            {
                widget->mMouseWheelEvent(mouse.wheelDelta());
                // WARNING : for now, we always propagate the wheel event
                // because it's convenient for scrollview, but maybe this
                // needs to be changed, like having a specific boolean
                // for wheel event propagation
                widget = widget->parent();
            } while(widget);

            time_since_last_button_down_ = 0.0f;
        }
    }
}

void UISystem::update(float elapsedTime)
{
    for(auto* widget : resized_widgets_)
        widget->isResizing = false;

    if(inputs_.mouse().is_button_up(Mouse::Button::Left))
        resized_widgets_.clear();

    auto* workingNode = &root_;

    // If there's one modal Widget, they are the only one processing events
    if(!modal_widgets_.empty())
        workingNode = modal_widgets_.children_.back().get();

    for(auto& node : *workingNode)
    {
        node->get()->checkForResizeEvent();
    }

    handleMouseEnter(elapsedTime, *workingNode);
    handleMouseExit(elapsedTime, *workingNode);
    handleMouseButtonDown(elapsedTime, *workingNode);
    handleMouseButtonUp(elapsedTime, *workingNode);
    handleWheelEvent(elapsedTime, *workingNode);
    handleMouseClick(elapsedTime, *workingNode);
    handle_drag_event(elapsedTime, *workingNode);

    // TODO : I'm not too sure about this behavior, maybe I should
    // react to key press only if the widget is focused or something
    for(auto& node : *workingNode)
    {
        if(inputs_.keyboard().is_key_pressed())
        {
            node->get()->on_key_pressed(inputs_);
        }
    }

    cursor_updated_ = false;

    for(auto& node : *workingNode)
        checkEdges(*node->get());

    if(!cursor_updated_)
    {
        if(Cursor::get() != Cursor::DefaultCursor::Hand)
            Cursor::set(Cursor::DefaultCursor::Arrow);
    }

    // Handling resizing

    if(inputs_.mouse().is_button_pressed(Mouse::Button::Left))
    {
        for(auto* widget : resized_widgets_)
        {
            resizeWidget(widget);
        }
    }

    checkManualFocus(*workingNode);

    // We need to first update the ui system because some of them
    // might gets updated and destroyed the previous pointer used
    if(!workingNode->mValue->isDestroyed())
    {
        for(auto& child : *workingNode)
        {
            if(child.mValue->hasTooltip_)
            {
                if(child.mValue->startTooltipTimer_ >= 0.0f &&
                   child.mValue->startTooltipTimer_ <= 1.0f)
                {
                    child.mValue->startTooltipTimer_ += 0.015f;
                }
                else if(child.mValue->startTooltipTimer_ > 1.0f)
                {
                    auto& rectangle =
                        tooltip_widgets_.mValue->emplace_back<ui::Rectangle>();

                    auto& textWidget = rectangle.emplace_back<ui::Text>();
                    auto textWidth = textWidget.textWidth(child.mValue->tooltip_.c_str());

                    // We just want to have some space on the left/right
                    // of the text
                    textWidth += 20;

                    rectangle.setLeft(inputs_.mouse().x() - textWidth / 2.0f);
                    rectangle.setTop(static_cast<float>(inputs_.mouse().y() - 50));

                    rectangle.setColor(Color(90, 90, 100));
                    rectangle.setHeight(30);
                    rectangle.setWidth(textWidth);
                    rectangle.setRadius(8.0f);

                    textWidget.setAnchorsToFillParentSpace();
                    textWidget.setText(child.mValue->tooltip_.c_str());
                    child.mValue->startTooltipTimer_ = -1.0f;

                    toolTips_.emplace(child.mValue.get(), &rectangle);
                }
            }
            if(child.mValue->isEnabled())
            {
                workingNode->mValue->update(elapsedTime);
            }
        }
    }

    // We display all regular widgets
    root_.mValue->actual_paint(renderer_);
    // We display all modal widget

    modal_widgets_.mValue->actual_paint(renderer_);

    tooltip_widgets_.mValue->actual_paint(renderer_);

    // But we only process events for the top modal widget if there's at least
    // one. Otherwise we process events for every regular widget

    // We destroy widgets only at the end of the update process
    removeDestroyedWidgets(*workingNode);
}

static void setResizeEast(ui::Widget* widget, float x, bool inPercentage)
{
    const auto localWidth = x - widget->real_x();

    if(inPercentage)
        widget->setWidth(localWidth / widget->parent()->width() * 100.0f, true);
    else
        widget->setWidth(localWidth);
}

static void setResizeWest(ui::Widget* widget,
                          float       resizeStartX,
                          float       previousWidth,
                          float       x,
                          bool        inPercentage)
{
    const auto localWidth = resizeStartX - x + previousWidth;
    std::cout << "West Parent " << widget->parent()->width() << std::endl;
    if(inPercentage)
    {
        widget->setWidth(localWidth / widget->parent()->width() * 100.0f, true);
        widget->x(x / widget->parent()->width() * 100.0f, true);
    }
    else
    {
        widget->x(x);
        widget->setWidth(localWidth);
    }
}

static void setResizeNorth(ui::Widget* widget,
                           float       y,
                           float       previousHeight,
                           float       resizeStartY,
                           bool        inPercentage)
{
    const auto localHeight = resizeStartY - y + previousHeight;

    if(inPercentage)
    {
        widget->y(y / widget->parent()->height() * 100.0f, true);
        widget->setHeight(localHeight / widget->parent()->height() * 100.0f, true);
    }
    else
    {
        widget->y(y);
        widget->setHeight(localHeight);
    }
}

static void setResizeSouth(ui::Widget* widget, float y, bool inPercentage)
{
    const auto localHeight = y - widget->real_y();

    if(inPercentage)
        widget->setHeight(localHeight / widget->parent()->height() * 100.0f);
    else
        widget->setHeight(localHeight);
}

void UISystem::resizeWidget(ui::Widget* widget)
{
    switch(widget->currentResize_)
    {
        case ui::Widget::ResizeEnum::E:

            setResizeEast(widget, static_cast<float>(inputs_.mouse().x()),
                          widget->resize_.rightInPercentage);
            break;

        case ui::Widget::ResizeEnum::W:

            setResizeWest(widget, resize_start_x_, widget_resize_width_before_,
                          static_cast<float>(inputs_.mouse().x()),
                          widget->resize_.leftInPercentage);

            break;

        case ui::Widget::ResizeEnum::N:

            setResizeNorth(widget, static_cast<float>(inputs_.mouse().y()),
                           widget_resize_height_before_, resize_start_y_,
                           widget->resize_.topInPercentage);

            break;

        case ui::Widget::ResizeEnum::S:

            setResizeSouth(widget, static_cast<float>(inputs_.mouse().y()),
                           widget->resize_.bottomInPercentage);

            break;

        case ui::Widget::ResizeEnum::NE:
            setResizeNorth(widget, static_cast<float>(inputs_.mouse().y()),
                           widget_resize_height_before_, resize_start_y_,
                           widget->resize_.topInPercentage);
            setResizeEast(widget, static_cast<float>(inputs_.mouse().x()),
                          widget->resize_.rightInPercentage);
            break;

        case ui::Widget::ResizeEnum::NW:
            setResizeNorth(widget, static_cast<float>(inputs_.mouse().y()),
                           widget_resize_height_before_, resize_start_y_,
                           widget->resize_.topInPercentage);
            setResizeWest(widget, resize_start_x_,
                          static_cast<float>(widget_resize_width_before_),
                          static_cast<float>(inputs_.mouse().x()),
                          widget->resize_.leftInPercentage);
            break;

        case ui::Widget::ResizeEnum::SE:
            setResizeSouth(widget, static_cast<float>(inputs_.mouse().y()),
                           widget->resize_.bottomInPercentage);
            setResizeEast(widget, static_cast<float>(inputs_.mouse().x()),
                          widget->resize_.rightInPercentage);
            break;

        case ui::Widget::ResizeEnum::SW:
            setResizeSouth(widget, static_cast<float>(inputs_.mouse().y()),
                           widget->resize_.bottomInPercentage);
            setResizeWest(widget, resize_start_x_, widget_resize_width_before_,
                          static_cast<float>(inputs_.mouse().x()),
                          widget->resize_.leftInPercentage);
            break;
    }
    widget->isResizing = true;
    widget->resizingEvent();
}

ui::Widget& UISystem::emplace_back(Window& window)
{
    window.resized += [&](const int width, const int height)
    {
        root_.mValue->setWidth(static_cast<float>(width));
        root_.mValue->setHeight(static_cast<float>(height));

        modal_widgets_.mValue->setWidth(static_cast<float>(width));
        modal_widgets_.mValue->setHeight(static_cast<float>(height));

        tooltip_widgets_.mValue->setWidth(static_cast<float>(width));
        tooltip_widgets_.mValue->setHeight(static_cast<float>(height));
    };

    root_.mValue->setWidth(static_cast<float>(window.width()));
    root_.mValue->setHeight(static_cast<float>(window.height()));

    auto& widget     = root_.mValue->emplace_back<ui::Widget>();
    widget.uiSystem_ = this;

    widget.setWidth(static_cast<float>(window.width()));
    widget.setHeight(static_cast<float>(window.height()));

    widget.window_ = &window;

    return widget;
}

ui::Widget& UISystem::addModalWidget(Window& window)
{
    modal_widgets_.mValue->setWidth(static_cast<float>(window.width()));
    modal_widgets_.mValue->setHeight(static_cast<float>(window.height()));

    auto& widget     = modal_widgets_.mValue->emplace_back<ui::Widget>();
    widget.uiSystem_ = this;
    widget.setAnchorsToFillParentSpace();
    widget.window_ = &window;
    // widget.setWidth(window_.width());
    // widget.setHeight(window_.height());

    return widget;
}

void UISystem::handleMouseEnter(float elapsedTime, NodeWidget& nodeWidget)
{
    for(auto& childNode : nodeWidget)
    {
        auto* widget = childNode->get();

        // We skip a deactivated widget
        if(!widget->activity_)
            continue;

        if(widget->contains(inputs_.mouse().x(), inputs_.mouse().y()))
        {
            // We check this boolean just to make sure we don't trigger the mouseHasEntered function twice
            if(!widget->mouseHasEntered_)
            {
                widget->onMouseEnter_(inputs_.mouse().x(), inputs_.mouse().y());
                widget->mouseHasEntered_ = true;
                widget->mouseHasExited_  = false;
                if(widget->hasTooltip_)
                {
                    widget->startTooltipTimer_ = 0.0f;
                }
            }
        }
    }
}

void UISystem::handleMouseExit(float elapsedTime, NodeWidget& nodeWidget)
{
    for(auto& node : nodeWidget)
    {
        auto* widget = node->get();

        // We skip a deactivated widget
        if(!widget->activity_)
            continue;

        if(!widget->contains(inputs_.mouse().x(), inputs_.mouse().y()))
        {
            // We check this boolean just to make sure we don't trigger the mouseHasEntered function twice
            if(!widget->mouseHasExited_)
            {
                widget->onMouseExit_(inputs_.mouse().x(), inputs_.mouse().y());
                widget->mouseHasEntered_   = false;
                widget->mouseHasExited_    = true;
                widget->startTooltipTimer_ = -1.0f;

                if(toolTips_.contains(widget))
                {
                    toolTips_[widget]->destroy();
                    toolTips_.erase(widget);
                }
            }
        }
    }
}
