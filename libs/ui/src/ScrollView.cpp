#include "corgi/systems/UISystem.h"

#include <corgi/math/MathUtils.h>
#include <corgi/rendering/renderer.h>
#include <corgi/systems/SpriteRendererSystem.h>
#include <corgi/ui/ScrollView.h>
#include <corgi/ui/Slider.h>
#include <corgi/utils/ResourcesCache.h>

namespace corgi::ui
{
ScrollView::ScrollView()
{
    isViewport = true;
}

Widget* ScrollView::content()
{
    return content_;
}

void ScrollView::init()
{
    mMaterial = *ResourcesCache::get<Material>(
        "corgi/Materials/ui/uiRectangle.mat");

    mMouseWheelEvent += [&](int wheelDelta)
    {
        // We get the content children
        auto c = content_->getChildrenAsWidget()[0];

        auto h  = height();
        auto ch = c->height();

        // We don't move the scrollbar if the content height is
        // smaller that the area
        if(ch < h)
            return;

        setScrollbarOffset(scrollbarOffset - wheelDelta * 76.0f * h / ch);
    };

    content_ = emplaceBack<Widget>();
    content_->setAnchorsToFillParentSpace();

    mVerticalScrollbarContainer = &emplace_back<Rectangle>();
    mVerticalScrollbarHandle    = &emplace_back<Rectangle>();

    mVerticalScrollbarHandle->mouse_drag_start_event() += [&](const Mouse& mouse)
    {
        startDragY  = mouse.y();
        startOffset = scrollbarOffset;
        mIsDragging = true;
    };

    mVerticalScrollbarHandle->mouse_drag_end_event() += [&](const Mouse& mouse)
    {
        mIsDragging = false;
    };

    mVerticalScrollbarHandle->onMouseEnter() += [&](int x, int y)
    {
        mVerticalScrollbarHandle->setColor(selectedHandleColor);
    };

    mVerticalScrollbarHandle->onMouseExit() += [&](int x, int y)
    {
        if(mIsDragging)
            mVerticalScrollbarHandle->setColor(selectedHandleColor);
        else
            mVerticalScrollbarHandle->setColor(handleColor);
    };

    mVerticalScrollbarHandle->mouse_drag_event() += [&](const Mouse& mouse)
    {
        mVerticalScrollbarHandle->setColor(selectedHandleColor);
        if(!getChildren().empty())
        {
            // We get back the content child
            auto child = content_->getChildrenAsWidget()[0];

            auto h  = height();
            auto ch = child->height();

            if(ch > h)
            {
                float range = h - ((h / ch) * h);

                setScrollbarOffset(startOffset + mouse.y() - startDragY);
            }
        }
    };
}

void ScrollView::setScrollbarOffset(float newScrollbarOffset)
{
    // The 3 children is actually the one with the content, the first 2 are
    // the scrollbars
    auto child = content_->getChildrenAsWidget()[0];

    auto h  = height();
    auto ch = child->height();

    // If the equation result is inferior to 0, it means we don't want toshow the scrollbar
    float range = corgi::math::max(0.0f, h - ((h / ch) * h));

    scrollbarOffset = corgi::math::clamp(newScrollbarOffset, 0.0f, range);
}

static void updateMaterialRecursively(Widget* widget)
{
    widget->mMaterial.stencil_fail(StencilOp::Keep);
    widget->mMaterial.stencil_success_depth_fail(StencilOp::Keep);
    widget->mMaterial.stencil_success_depth_success(StencilOp::Keep);
    widget->mMaterial.enable_stencil_test(true);

    widget->mMaterial.stencil_test  = StencilTest::Equal;
    widget->mMaterial.stencil_value = 1;

    for(auto& child : widget->getChildren())
    {
        auto childWidget = child.get()->mValue.get();
        updateMaterialRecursively(childWidget);
    }
}

void ScrollView::paint(corgi::Renderer& renderer)
{
    // We call this function just to make sure the scrollBarOffset is within bounds
    setScrollbarOffset(scrollbarOffset);

    //(mWidthScrollingBar)?mHorizontalSlider->enable(): mHorizontalSlider->disable();

    if(!getChildren().empty())
    {
        // We get back the content child
        auto child = content_->getChildrenAsWidget()[0];

        auto h  = height();
        auto ch = child->height();

        float verticalScrollBarWidth  = 14;
        float verticalScrollBarOffset = 0;

        float verticalScrollBarHandleOffset = 0;

        //If child height is bigger than our scrollview, we show the verticalSlider
        if(child->height() > height())
        {
            mVerticalScrollbarContainer->enable();
            mVerticalScrollbarContainer->setRight(verticalScrollBarOffset);
            mVerticalScrollbarContainer->setLeft(width() - verticalScrollBarOffset -
                                                 verticalScrollBarWidth);
            mVerticalScrollbarContainer->setTop(0);
            mVerticalScrollbarContainer->setBottom(0);
            mVerticalScrollbarContainer->setRadius(0);
            mVerticalScrollbarContainer->setColor(containerColor);

            mVerticalScrollbarHandle->enable();

            mVerticalScrollbarHandle->setRight(verticalScrollBarOffset +
                                               verticalScrollBarHandleOffset);
            mVerticalScrollbarHandle->setLeft(width() - verticalScrollBarOffset -
                                              verticalScrollBarWidth +
                                              verticalScrollBarHandleOffset);

            mVerticalScrollbarHandle->setTop(scrollbarOffset);
            mVerticalScrollbarHandle->setHeight((h / ch) * h);
            mVerticalScrollbarHandle->setRadius(0);
        }
        else
        {
            mVerticalScrollbarContainer->disable();
            mVerticalScrollbarHandle->disable();
        }
    }

    // So here I need to draw a rectangle on the
    // stencil buffer

    mMaterial.enable_depth_test(true);
    mMaterial.write_color(false);
    mMaterial.enable_stencil_test(true);

    // We replace everything. Should put a 1 into every pixel from our rectangle I guess?
    mMaterial.stencil_test = StencilTest::Always;
    mMaterial.stencil_fail(StencilOp::Replace);
    mMaterial.stencil_success_depth_fail(StencilOp::Replace);
    mMaterial.stencil_success_depth_success(StencilOp::Replace);

    mMaterial.set_uniform("uMainColor", Vec4(1.0, 0.0f, 0.0f, 1.0f));

    mMaterial.set_uniform("uRectangleWidth", width());
    mMaterial.set_uniform("uRectangleHeight", height());
    mMaterial.set_uniform("uRadius", 1);

    // TO BE FAIR ... that whole thing probably should be in widget
    // so I don't have to do it differently everytime
    renderer.window_draw_list().add_mesh(
        SpriteRendererSystem::getUiMesh(0),
        Matrix::translation(real_x(), real_y(), 0.0f) *
        Matrix::scale(width(), height(), 1.0f),
        mMaterial, *window_);

    // The problem here is that I might want to erase the stencil buffer
    // after
    int  i = 0;
    auto c = content_->getChildrenAsWidget()[0];

    auto h  = height();
    auto ch = c->height();

    c->y(-scrollbarOffset / height() * ch);
    updateMaterialRecursively(c);
    c->paint(renderer);
}

void ScrollView::actual_paint(Renderer& renderer)
{
    Widget::actual_paint(renderer);
    // Once we're done we reset the stencil buffer
    renderer.windowDrawList().resetStencilBuffer();
}
}    // namespace corgi::ui
