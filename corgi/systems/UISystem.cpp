#include <corgi/main/Cursor.h>
#include <corgi/main/Window.h>
#include <corgi/math/MathUtils.h>
#include <corgi/rendering/renderer.h>
#include <corgi/systems/UISystem.h>
#include <corgi/ui/Rectangle.h>
#include <corgi/ui/Text.h>
#include <corgi/utils/ResourcesCache.h>

using namespace corgi;

UISystem::UISystem(Window& window, Renderer& renderer, const Inputs& inputs)
    : mInputs(inputs)
    , mWindow(window)
    , renderer_(renderer)
{
    root_.mValue.reset(new ui::Widget());
    root_.mValue->mNode      = &root_;
    root_.mValue->isViewport = true;
    root_.mValue->setWidth(static_cast<float>(window.width()));
    root_.mValue->setHeight(static_cast<float>(window.height()));
    root_.mValue->uiSystem_ = this;

    //mRoot->get()->mViewport = &mRoot;

    modalWidgets_.mValue.reset(new ui::Widget());
    modalWidgets_.mValue->mNode      = &modalWidgets_;
    modalWidgets_.mValue->isViewport = true;
    modalWidgets_.mValue->setWidth(static_cast<float>(window.width()));
    modalWidgets_.mValue->setHeight(static_cast<float>(window.height()));
    modalWidgets_.mValue->uiSystem_ = this;

    tooltipWidgets_.mValue.reset(new ui::Widget());
    tooltipWidgets_.mValue->mNode      = &tooltipWidgets_;
    tooltipWidgets_.mValue->isViewport = true;
    tooltipWidgets_.mValue->setWidth(static_cast<float>(window.width()));
    tooltipWidgets_.mValue->setHeight(static_cast<float>(window.height()));
    tooltipWidgets_.mValue->uiSystem_ = this;

    //mRoot->get()->mViewport = &mRoot;

    mWindow.resized += [&](const int width, const int height)
    {
        root_.mValue->setWidth(static_cast<float>(width));
        root_.mValue->setHeight(static_cast<float>(height));

        modalWidgets_.mValue->setWidth(static_cast<float>(width));
        modalWidgets_.mValue->setHeight(static_cast<float>(height));

        tooltipWidgets_.mValue->setWidth(static_cast<float>(width));
        tooltipWidgets_.mValue->setHeight(static_cast<float>(height));
    };
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
    const auto& mouse = mInputs.mouse();
    mTimeSinceLastButtonDown += elapsedTime;
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
                mPushedWidget.push_back(widget);
                widget->mPropagateEvent ? widget = widget->parent() : widget = nullptr;
            } while(widget);

            mTimeSinceLastButtonDown = 0.0f;
        }
    }
}

void UISystem::handleMouseClick(float elapsedTime, NodeWidget& nodeWidget)
{
    const auto& mouse = mInputs.mouse();

    if(mouse.is_button_up(Mouse::Button::Left))
    {
        auto        widget         = findDeepestWidget(mouse, nodeWidget);
        ui::Widget* previousWidget = nullptr;

        if(widget)
        {
            auto now = std::chrono::steady_clock::now();
            auto ms  = std::chrono::duration_cast<std::chrono::milliseconds>(
                          now - mPreviousClick)
                          .count();

            do
            {
                previousWidget = widget;
                auto it = std::find(mPushedWidget.begin(), mPushedWidget.end(), widget);

                if(it != mPushedWidget.end())
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

            mPreviousClick = now;

            // The last propagated widget will trigger the focus event
            // (if there's a change of course)
            if(mFocusedWidget != nullptr)
            {
                if(mFocusedWidget != previousWidget)
                {
                    mFocusedWidget->mFocusOutEvent();
                    mFocusedWidget->mFocus = false;
                }
            }

            if(mFocusedWidget != previousWidget)
            {
                previousWidget->mFocusInEvent();
                mFocusedWidget         = previousWidget;
                previousWidget->mFocus = true;
            }
        }

        mPushedWidget.clear();
    }
}

void UISystem::searchIfDestroyedWidgetWasFocused(ui::Widget* widget)
{
    if(mFocusedWidget == widget)
    {
        mFocusedWidget = nullptr;
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
        if(mFocusedWidget)
        {
            mFocusedWidget->mFocusOutEvent();
            mFocusedWidget->mFocus = false;
        }

        focusIn->mFocusInEvent();
        mFocusedWidget         = focusIn;
        mFocusedWidget->mFocus = true;
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

    return math::in_range(static_cast<float>(mInputs.mouse().x()),
                          widget.real_x() + widget.width(),
                          widget.real_x() + widget.width() + edgeOffset_) &&
           math::in_range(static_cast<float>(mInputs.mouse().y()), widget.real_y(),
                          widget.real_y() + widget.height());
}

bool UISystem::checkLeftEdge(const ui::Widget& widget) const noexcept
{
    if(!widget.resize_.left)
        return false;

    return math::in_range(static_cast<float>(mInputs.mouse().x()),
                          widget.real_x() - edgeOffset_, widget.real_x()) &&
           math::in_range(static_cast<float>(mInputs.mouse().y()),
                          widget.real_y() - edgeOffset_,
                          widget.real_y() + widget.height() + edgeOffset_);
}

bool UISystem::checkTopEdge(const ui::Widget& widget) const noexcept
{
    if(!widget.resize_.top)
        return false;

    return math::in_range(static_cast<float>(mInputs.mouse().x()),
                          widget.real_x() - edgeOffset_,
                          widget.real_x() + widget.width() + edgeOffset_) &&
           math::in_range(static_cast<float>(mInputs.mouse().y()),
                          widget.real_y() - edgeOffset_, widget.real_y());
}

bool UISystem::checkBottomEdge(const ui::Widget& widget) const noexcept
{
    if(!widget.resize_.bottom)
        return false;

    return math::in_range(static_cast<float>(mInputs.mouse().x()), widget.real_x(),
                          widget.real_x() + widget.width()) &&
           math::in_range(static_cast<float>(mInputs.mouse().y()),
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
        if(mInputs.mouse().is_button_down(Mouse::Button::Left))
        {
            widget.currentResize_ = corgi::ui::Widget::ResizeEnum::W;
            startResizing(widget);
        }
        cursorUpdated_ = true;
        return Cursor::set(Cursor::DefaultCursor::SizeWE);
    }

    if(!left && right && !top && !bottom)    // Right edge
    {
        if(mInputs.mouse().is_button_down(Mouse::Button::Left))
        {
            widget.currentResize_ = corgi::ui::Widget::ResizeEnum::E;
            startResizing(widget);
        }
        cursorUpdated_ = true;
        return Cursor::set(Cursor::DefaultCursor::SizeWE);
    }

    if(left && !right && top && !bottom)    // Top left corner
    {
        if(mInputs.mouse().is_button_down(Mouse::Button::Left))
        {
            widget.currentResize_ = corgi::ui::Widget::ResizeEnum::NW;
            startResizing(widget);
        }
        cursorUpdated_ = true;
        return Cursor::set(Cursor::DefaultCursor::SizeNWSE);
    }

    if(left && !right && !top && bottom)    // Bottom left corner
    {
        if(mInputs.mouse().is_button_down(Mouse::Button::Left))
        {
            widget.currentResize_ = corgi::ui::Widget::ResizeEnum::SW;
            startResizing(widget);
        }
        cursorUpdated_ = true;
        return Cursor::set(Cursor::DefaultCursor::SizeNESW);
    }

    if(!left && !right && !top && bottom)    //Bottom edge
    {
        if(mInputs.mouse().is_button_down(Mouse::Button::Left))
        {
            widget.currentResize_ = corgi::ui::Widget::ResizeEnum::S;
            startResizing(widget);
        }
        cursorUpdated_ = true;
        return Cursor::set(Cursor::DefaultCursor::SizeNS);
    }

    if(!left && !right && top && !bottom)    // Top edge
    {
        if(mInputs.mouse().is_button_down(Mouse::Button::Left))
        {
            widget.currentResize_ = corgi::ui::Widget::ResizeEnum::N;
            startResizing(widget);
        }
        cursorUpdated_ = true;
        return Cursor::set(Cursor::DefaultCursor::SizeNS);
    }

    if(!left && right && top && !bottom)    // Top Right corner
    {
        if(mInputs.mouse().is_button_down(Mouse::Button::Left))
        {
            widget.currentResize_ = corgi::ui::Widget::ResizeEnum::NE;
            startResizing(widget);
        }
        cursorUpdated_ = true;
        return Cursor::set(Cursor::DefaultCursor::SizeNESW);
    }

    if(!left && right && !top && bottom)    // Bottom right corner
    {
        if(mInputs.mouse().is_button_down(Mouse::Button::Left))
        {
            widget.currentResize_ = corgi::ui::Widget::ResizeEnum::SE;
            startResizing(widget);
        }
        cursorUpdated_ = true;
        return Cursor::set(Cursor::DefaultCursor::SizeNWSE);
    }
}

void UISystem::startResizing(ui::Widget& widget)
{
    resizedWidgets_.push_back(&widget);
    resizeStartX_ = static_cast<float>(mInputs.mouse().x());
    resizeStartY_ = static_cast<float>(mInputs.mouse().y());

    widgetResizeHeightBefore_ = widget.height();
    widgetResizeWidthBefore_  = widget.width();
}

void UISystem::handleMouseButtonUp(float elapsedTime, NodeWidget& nodeWidget)
{
    const auto& mouse = mInputs.mouse();
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
    const auto& mouse = mInputs.mouse();

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

            mTimeSinceLastButtonDown = 0.0f;
        }
    }
}

void UISystem::update(float elapsedTime)
{
    for(auto* widget : resizedWidgets_)
        widget->isResizing = false;

    if(mInputs.mouse().is_button_up(Mouse::Button::Left))
        resizedWidgets_.clear();

    auto* workingNode = &root_;

    // If there's one modal Widget, they are the only one processing events
    if(!modalWidgets_.empty())
        workingNode = modalWidgets_.children_.back().get();

    handleMouseEnter(elapsedTime, *workingNode);
    handleMouseExit(elapsedTime, *workingNode);
    handleMouseButtonDown(elapsedTime, *workingNode);
    handleMouseButtonUp(elapsedTime, *workingNode);
    handleWheelEvent(elapsedTime, *workingNode);
    handleMouseClick(elapsedTime, *workingNode);

    cursorUpdated_ = false;

    for(auto& node : *workingNode)
        checkEdges(*node->get());

    if(!cursorUpdated_)
    {
        if(Cursor::get() != Cursor::DefaultCursor::Hand)
            Cursor::set(Cursor::DefaultCursor::Arrow);
    }

    // Handling resizing

    if(mInputs.mouse().is_button_pressed(Mouse::Button::Left))
    {
        for(auto* widget : resizedWidgets_)
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
                        tooltipWidgets_.mValue->emplace_back<ui::Rectangle>();

                    auto& textWidget = rectangle.emplace_back<ui::Text>();
                    auto  textWidth  = textWidget.textWidth(child.mValue->tooltip_);

                    // We just want to have some space on the left/right
                    // of the text
                    textWidth += 20;

                    rectangle.setLeft(mInputs.mouse().x() - textWidth / 2.0f);
                    rectangle.setTop(mInputs.mouse().y() - 50);

                    rectangle.setColor(Color(90, 90, 100));
                    rectangle.setHeight(30);
                    rectangle.setWidth(textWidth);
                    rectangle.setRadius(8);

                    textWidget.setAnchorsToFillParentSpace();
                    textWidget.setText(child.mValue->tooltip_);
                    child.mValue->startTooltipTimer_ = -1.0f;

                    toolTips_.emplace(child.mValue.get(), &rectangle);
                }
            }
        }

        workingNode->mValue->update_event(mInputs, 0.15f);
    }

    // We display all regular widgets
    root_.mValue->actual_paint(renderer_);
    // We display all modal widget

    modalWidgets_.mValue->actual_paint(renderer_);

    tooltipWidgets_.mValue->actual_paint(renderer_);

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

            setResizeEast(widget, mInputs.mouse().x(), widget->resize_.rightInPercentage);
            break;

        case ui::Widget::ResizeEnum::W:

            setResizeWest(widget, resizeStartX_, widgetResizeWidthBefore_,
                          static_cast<float>(mInputs.mouse().x()),
                          widget->resize_.leftInPercentage);

            break;

        case ui::Widget::ResizeEnum::N:

            setResizeNorth(widget, static_cast<float>(mInputs.mouse().y()),
                           widgetResizeHeightBefore_, resizeStartY_,
                           widget->resize_.topInPercentage);

            break;

        case ui::Widget::ResizeEnum::S:

            setResizeSouth(widget, static_cast<float>(mInputs.mouse().y()),
                           widget->resize_.bottomInPercentage);

            break;

        case ui::Widget::ResizeEnum::NE:
            setResizeNorth(widget, static_cast<float>(mInputs.mouse().y()),
                           widgetResizeHeightBefore_, resizeStartY_,
                           widget->resize_.topInPercentage);
            setResizeEast(widget, mInputs.mouse().x(), widget->resize_.rightInPercentage);
            break;

        case ui::Widget::ResizeEnum::NW:
            setResizeNorth(widget, static_cast<float>(mInputs.mouse().y()),
                           widgetResizeHeightBefore_, resizeStartY_,
                           widget->resize_.topInPercentage);
            setResizeWest(widget, resizeStartX_, widgetResizeWidthBefore_,
                          static_cast<float>(mInputs.mouse().x()),
                          widget->resize_.leftInPercentage);
            break;

        case ui::Widget::ResizeEnum::SE:
            setResizeSouth(widget, static_cast<float>(mInputs.mouse().y()),
                           widget->resize_.bottomInPercentage);
            setResizeEast(widget, mInputs.mouse().x(), widget->resize_.rightInPercentage);
            break;

        case ui::Widget::ResizeEnum::SW:
            setResizeSouth(widget, static_cast<float>(mInputs.mouse().y()),
                           widget->resize_.bottomInPercentage);
            setResizeWest(widget, resizeStartX_, widgetResizeWidthBefore_,
                          static_cast<float>(mInputs.mouse().x()),
                          widget->resize_.leftInPercentage);
            break;
    }
    widget->isResizing = true;
    widget->resizingEvent();
}

ui::Widget& UISystem::emplace_back()
{
    root_.mValue->setWidth(mWindow.width());
    root_.mValue->setHeight(mWindow.height());

    auto& widget     = root_.mValue->emplace_back<ui::Widget>();
    widget.uiSystem_ = this;

    widget.setWidth(mWindow.width());
    widget.setHeight(mWindow.height());

    return widget;
}

ui::Widget& UISystem::addModalWidget()
{
    modalWidgets_.mValue->setWidth(mWindow.width());
    modalWidgets_.mValue->setHeight(mWindow.height());

    auto& widget     = modalWidgets_.mValue->emplace_back<ui::Widget>();
    widget.uiSystem_ = this;
    widget.setAnchorsToFillParentSpace();
    // widget.setWidth(mWindow.width());
    // widget.setHeight(mWindow.height());

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

        if(widget->contains(mInputs.mouse().x(), mInputs.mouse().y()))
        {
            // We check this boolean just to make sure we don't trigger the mouseHasEntered function twice
            if(!widget->mouseHasEntered_)
            {
                widget->onMouseEnter_(mInputs.mouse().x(), mInputs.mouse().y());
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

        if(!widget->contains(mInputs.mouse().x(), mInputs.mouse().y()))
        {
            // We check this boolean just to make sure we don't trigger the mouseHasEntered function twice
            if(!widget->mouseHasExited_)
            {
                widget->onMouseExit_(mInputs.mouse().x(), mInputs.mouse().y());
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
