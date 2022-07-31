#include "corgi/systems/UISystem.h"

#include <corgi/rendering/Material.h>
#include <corgi/rendering/renderer.h>
#include <corgi/rendering/texture.h>
#include <corgi/systems/SpriteRendererSystem.h>
#include <corgi/ui/Image.h>
#include <corgi/utils/ResourcesCache.h>

namespace corgi::ui
{
Image::Image(Texture* image)
{
    texture_  = image;
    mMaterial =
        *ResourcesCache::get<Material>(
            "corgi/materials/ui/unlit_ui_sprite.mat");
    if(!texture_)
        return;
    width_  = static_cast<float>(texture_->width());
    height_ = static_cast<float>(texture_->height());
}

const Texture* Image::texture() const noexcept
{
    return texture_;
}

void Image::paint(Renderer& renderer)
{
    if(!texture_)
        return;

    static const SimpleString sprite_str     = "sprite";
    static const SimpleString sprite_tex_str = "sprite_tex";
    static const SimpleString flip_x_str     = "flip_x";

    float newWidth  = width();
    float newHeight = height();

    float aspectRatio = float(texture_->width()) / float(texture_->height());
    float ratio       = newWidth / float(texture_->width());
    float dif;

    switch(stretch_)
    {
        case Stretch::KeepAspectRatio :

            newHeight = float(texture_->height()) * ratio;

            if(newHeight >= height())
            {
                dif = newHeight - height();
                newWidth -= aspectRatio * dif;
                newHeight = height();
            }
            break;

        case Stretch::Fill :
            break;
    }

    // I kinda wonder if I could/should send int/unsigned to the shader
    mMaterial.set_uniform(flip_x_str.c_str(), 0);

    mMaterial.set_uniform("sprite_width", texture_->width());
    mMaterial.set_uniform("sprite_height", texture_->height());

    mMaterial.set_uniform("texture_width", texture_->width());
    mMaterial.set_uniform("texture_height", texture_->height());

    mMaterial.set_uniform("width", newWidth);
    mMaterial.set_uniform("height", newHeight);

    // TODO : Try to use a vec2 for this
    mMaterial.set_uniform("pivot_x", 0.00f);
    mMaterial.set_uniform("pivot_y", 0.00f);

    mMaterial.set_uniform("offset_x", 0u);
    mMaterial.set_uniform("offset_y", 0u);

    mMaterial.set_texture(0, *texture_);

    mMaterial.enable_depth_test(false);

    //renderer.window_draw_list().add_rectangle(real_x(), real_y(), width_,height_, *texture_);

    renderer.window_draw_list().add_mesh(
        SpriteRendererSystem::getUiMesh(0),
        Matrix::translation(real_x() + (width() - newWidth) / 2.0f,
                            real_y() + (height() - newHeight) / 2.0f, 0.0f),
        mMaterial, *window_);
}
}    // namespace corgi::ui
