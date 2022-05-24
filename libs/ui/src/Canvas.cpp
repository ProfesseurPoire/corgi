#include <corgi/ui/Canvas.h>

using namespace corgi::ui;

void Canvas::updateEvents(const Inputs& inputs)
{
    //const auto& mouse = inputs.mouse();

    //// We look for the widgets that contains the mouse cursor
    //for (auto* child : mRoot)
    //{
    //	auto* widget = dynamic_cast<Widget*>(child);

    //	if (widget->contains(mouse.x(), mouse.y()))
    //	{
    //		mMouseButtonDownWidgets.push_back(widget);
    //	}
    //}

    //// We only fire the mouseDownEvent on the deeper widget
    //// to avoid firing it multiple time
    //int depth = -1;

    //Widget* selectedWidget = nullptr;

    //for (auto* mouseButtonWidget : mMouseButtonDownWidgets)
    //{
    //	if (mouseButtonWidget->depth() > depth)
    //	{
    //		depth = mouseButtonWidget->depth();
    //		selectedWidget = mouseButtonWidget;
    //	}
    //}

    ///*if (selectedWidget != nullptr)
    //	selectedWidget->mOnMouseDown(mouse.x(), mouse.y());*/
}

void Canvas::paint(Renderer& renderer)
{
    mRoot.actual_paint(renderer);
}
