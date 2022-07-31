#include <corgi/inputs/Inputs.h>
#include <corgi/main/Cursor.h>
#include <corgi/main/Game.h>
#include <corgi/rendering/renderer.h>
#include <corgi/rendering/texture.h>
#include <corgi/ui/Button.h>
#include <corgi/ui/Text.h>
#include <corgi/ui/UiUtils.h>
#include <corgi/utils/ResourcesCache.h>

using namespace corgi::ui;

void Button::init()
{
    mMaterial = *ResourcesCache::get<Material>("corgi/materials/ui/nine_slice.mat");
    text_     = &emplace_back<Text>();
    text_->text("Button");
    text_->mProcessEvent = false;

    mouse_drag_event_ += [&](const Mouse& mouse)
    {
        is_pressed_ = true;
    };

    onMouseEnter() += [&](int x, int y)
    {
        Cursor::set(Cursor::DefaultCursor::Hand);
        if(Game::instance().inputs().mouse().is_button_up(corgi::Mouse::Button::Left))
        {
            is_pressed_ = false;
            on_click_();
        }
    };

    onMouseExit() += [&](int x, int y)
    {
        Cursor::set(Cursor::DefaultCursor::Arrow);
        if(Game::instance().inputs().mouse().is_button_up(corgi::Mouse::Button::Left))
        {
            is_pressed_ = false;
        }
    };
}

void Button::paint(Renderer& renderer)
{
    DrawList::NineSlice ns;

    try
    {
        if(is_pressed_)
        {
            if(mMaterial._texture_uniforms.empty())
                mMaterial.add_texture(
                    *ResourcesCache::get<Texture>(
                        "corgi/textures/grey_pressed.tex"));
            else
                mMaterial.set_texture(0, *ResourcesCache::get<Texture>(
                                          "corgi/textures/grey_pressed.tex"));
        }
        else
        {
            if(mMaterial._texture_uniforms.empty())
                mMaterial.add_texture(
                    *ResourcesCache::get<Texture>(
                        "corgi/textures/grey.tex"));
            else
                mMaterial.set_texture(
                    0, *ResourcesCache::get<Texture>(
                        "corgi/textures/grey.tex"));
        }
    }
    catch(const std::exception& e)
    {
        //log_error("Error while loading resources in Button::paint : "+std::string(e.what()));
    }

    auto texture = mMaterial._texture_uniforms.begin()->texture;

    mMaterial.enable_depth_test(false);

    ns.width  = width();
    ns.height = height();

    ns.x = real_x();
    ns.y = real_y();

    ns.top_border_    = 10.0f;
    ns.bottom_border_ = 10.0f;
    ns.left_border_   = 10.0f;
    ns.right_border_  = 10.0f;

    ns.left_border_uv_ =
        static_cast<float>(ns.left_border_) / static_cast<float>(texture->width());
    ns.right_border_uv_ =
        static_cast<float>(ns.right_border_) / static_cast<float>(texture->width());
    ns.top_border_uv_ =
        static_cast<float>(ns.top_border_) / static_cast<float>(texture->height());
    ns.bottom_border_uv_ =
        static_cast<float>(ns.bottom_border_) / static_cast<float>(texture->height());

    mMaterial.set_uniform("offset", real_x(), real_y());
    mMaterial.set_uniform("dimensions", width(), height());
    mMaterial.set_uniform("pivot", 0.0f, 0.0f);
    mMaterial.set_uniform("borders", 30.0f, 30.0f, 30.0f, 31.0f);
    mMaterial.set_uniform("texture_dimensions", texture->width(), texture->height());

    mMaterial.enable_depth_test(false);
    mMaterial.set_uniform("main_color", 1.0f, 0.0f, 0.0f, 1.0f);

    renderer.window_draw_list().add_mesh(UiUtils::nine_slice_quad(), Matrix(), mMaterial,
                                         *window_);

    if(text_)
    {
        if(width() != text_->width())
        {
            text_->setWidth(width());
            text_->update_mesh();
        }

        if(height() != text_->height())
        {
            text_->setHeight(height());
            text_->update_mesh();
        }
    }
}

void Button::update(float elapsed_time)
{
    on_update_();
}
