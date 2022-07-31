#include <corgi/main/Window.h>
#include <corgi/rendering/WindowDrawList.h>
#include <corgi/rendering/texture.h>
#include <corgi/utils/ResourcesCache.h>

using namespace corgi;

void WindowDrawList::add_rectangle(const float x,
                                   const float y,
                                   const float width,
                                   const float height,
                                   const float r,
                                   const float g,
                                   const float b,
                                   const float a)
{
    DrawList::Rectangle rectangle;

    rectangle.material = *ResourcesCache::get<Material>(
        "corgi/materials/unlit/unlit_color.mat");

    rectangle.x      = x;
    rectangle.y      = static_cast<float>(window_->height()) - y - height;
    rectangle.width  = width;
    rectangle.height = height;
    rectangle.material.set_uniform("main_color", Vec4(r, g, b, a));
    rectangle.material.enable_depth_test(false);

    draw_list_.add_rectangle(rectangle);
}

void WindowDrawList::set_current_window(Window* window)
{
    window_ = window;
}

void WindowDrawList::add_rectangle(const float    x,
                                   const float    y,
                                   const float    width,
                                   const float    height,
                                   const Texture& texture
    )
{
    DrawList::Rectangle rectangle;

    rectangle.material = *ResourcesCache::get<Material>(
        "corgi/materials/unlit/unlit_texture.mat");

    rectangle.material.add_texture(texture);

    rectangle.x      = x;
    rectangle.y      = static_cast<float>(window_->height()) - y - height;
    rectangle.width  = width;
    rectangle.height = height;
    rectangle.material.enable_depth_test(false);

    draw_list_.add_rectangle(rectangle);
}

void WindowDrawList::resetStencilBuffer()
{
    draw_list_.resetStencilBuffer();
}

void WindowDrawList::add_rectangle(const float x, const float y, const Texture& texture)
{
    DrawList::Rectangle rectangle;

    rectangle.material = *ResourcesCache::get<Material>(
        "corgi/materials/unlit/unlit_texture.mat");
    rectangle.material.add_texture(texture);

    rectangle.x      = x;
    rectangle.y      = static_cast<float>(window_->height()) - y - texture.height();
    rectangle.width  = static_cast<float>(texture.width());
    rectangle.height = static_cast<float>(texture.height());
    rectangle.material.enable_depth_test(false);

    draw_list_.add_rectangle(rectangle);
}

void WindowDrawList::add_sprite(float x, float y, const Sprite sprite)
{
    DrawList::DrawListSprite dwsprite;
    dwsprite.sprite = sprite;

    dwsprite.material = *ResourcesCache::get<Material>(
        "corgi/materials/unlit/unlit_texture.mat");
    dwsprite.material.add_texture(*sprite.texture);

    dwsprite.material.is_transparent(true);
    dwsprite.material.enable_depth_test(false);

    dwsprite.x      = x;
    dwsprite.y      = static_cast<float>(window_->height()) - y;
    dwsprite.width  = static_cast<float>(sprite.width);
    dwsprite.height = static_cast<float>(sprite.height);

    draw_list_.add_sprite(dwsprite);
}

void WindowDrawList::add_sprite(
    float        x,
    float        y,
    float        width,
    float        height,
    const Sprite sprite
    )
{
    DrawList::DrawListSprite dwsprite;
    dwsprite.sprite = sprite;

    dwsprite.material = *ResourcesCache::get<Material>(
        "corgi/materials/unlit/unlit_texture.mat");
    dwsprite.material.add_texture(*sprite.texture);

    dwsprite.material.is_transparent(true);

    dwsprite.x      = x;
    dwsprite.y      = static_cast<float>(window_->height()) - y;
    dwsprite.width  = width;
    dwsprite.height = height;
    dwsprite.material.enable_depth_test(false);

    draw_list_.add_sprite(dwsprite);
}

void WindowDrawList::add_nine_slice(DrawList::NineSlice nc)
{
    nc.y = static_cast<float>(window_->height()) - nc.y - nc.height;
    draw_list_.add_nine_slice(nc);
}

void WindowDrawList::add(DrawList::Text text)
{
    text.position.y =
        static_cast<float>(window_->height()) - text.position.y - text.dimensions.y;
    draw_list_.add(text);
}

void WindowDrawList::add_mesh(const Mesh&     mesh,
                              const Matrix&   model_matrix,
                              const Material& material,
                              corgi::Window&  window
    )
{
    draw_list_.add_mesh(mesh, model_matrix, material, &window);
}
