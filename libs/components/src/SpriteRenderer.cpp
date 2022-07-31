#include <corgi/components/SpriteRenderer.h>
#include <corgi/rendering/texture.h>
#include <corgi/utils/ResourcesCache.h>
#include <corgi/resources/Mesh.h>


namespace corgi
{
SpriteRenderer::SpriteRenderer()
    : _material(*ResourcesCache::get<Material>(
        "corgi/materials/unlit/unlit_sprite.mat"))
{ }

SpriteRenderer::SpriteRenderer(Texture& tex)
    : _material(*ResourcesCache::get<Material>(
        "corgi/materials/unlit/unlit_sprite.mat"))
{
    sprite(tex);
}

SpriteRenderer::SpriteRenderer(Sprite sprite, Pivot pivot)
    : _material(*ResourcesCache::get<Material>(
        "corgi/materials/unlit/unlit_sprite.mat"))
{
    static Vec2 pivots_[] = {
            {Vec2(0.5f, 0.5f)},    // Pivot::Center
            {Vec2(0.5f, 0.0f)},    // Pivot::Down
            {Vec2(1.0f, 0.5f)},    // Pivot::Right
            {Vec2(0.0f, 0.5f)},    // Pivot::Left
            {Vec2(0.5f, 1.0f)},    // Pivot::Up
            {Vec2(1.0f, 1.0f)},    // Pivot::RightUp
            {Vec2(0.0f, 1.0f)},    // Pivot::LeftUp
            {Vec2(0.0f, 0.0f)},    // Pivot::LeftDown
            {Vec2(1.0f, 0.0f)}     // Pivot::RightDown
        };

    _pivot_value = pivots_[static_cast<int>(pivot)];

    this->sprite(sprite);
}

void SpriteRenderer::sprite(Texture& texture)
{
    sprite_.width   = texture.width();
    sprite_.height  = texture.height();
    sprite_.texture = &texture;
    sprite(sprite_);
    _dirty = true;
}

void SpriteRenderer::flip_horizontal()
{
    _flipped_x = !_flipped_x;
}

void SpriteRenderer::flip_horizontal(bool value)
{
    _flipped_x = value;
    _dirty     = true;
}

void SpriteRenderer::flip_vertical()
{
    _flipped_y = !_flipped_y;
    _dirty     = true;
}

void SpriteRenderer::flip_vertical(bool value)
{
    _flipped_y = value;
    _dirty     = true;
}

void SpriteRenderer::material(const Material& material)
{
    _material = material;
    _dirty    = true;
}

const Material& SpriteRenderer::material() const
{
    return _material;
}

void SpriteRenderer::pivot(Pivot pivot)
{
    static Vec2 pivots_[] = {
            {Vec2(0.5f, 0.5f)},    // Pivot::Center
            {Vec2(0.5f, 0.0f)},    // Pivot::Down
            {Vec2(1.0f, 0.5f)},    // Pivot::Right
            {Vec2(0.0f, 0.5f)},    // Pivot::Left
            {Vec2(0.5f, 1.0f)},    // Pivot::Up
            {Vec2(1.0f, 1.0f)},    // Pivot::RightUp
            {Vec2(0.0f, 1.0f)},    // Pivot::LeftUp
            {Vec2(0.0f, 0.0f)},    // Pivot::LeftDown
            {Vec2(1.0f, 0.0f)}     // Pivot::RightDown
        };

    _pivot_value = pivots_[static_cast<int>(pivot)];
}

void SpriteRenderer::pivot(float x, float y)
{
    _pivot_value.x = x;
    _pivot_value.y = y;
    _dirty         = true;
}

void SpriteRenderer::sprite(Sprite sprite)
{
    if(sprite.texture == nullptr)
        return;

    if(sprite_ == sprite)
        return;

    // There's no flip information in the sprite so

    /*if(_flipped_x)
                    material.set_uniform(0, flip_x, 1);
            else
                    material.set_uniform(0, flip_x, 0);*/

    sprite_ = sprite;
    _dirty  = true;
}

Sprite& SpriteRenderer::sprite()
{
    return sprite_;
}

Vec2 SpriteRenderer::pivot() const noexcept
{
    return _pivot_value;
}

bool SpriteRenderer::is_horizontally_flipped() const noexcept
{
    return _flipped_x;
}

bool SpriteRenderer::is_vertically_flipped() const noexcept
{
    return _flipped_y;
}
}    // namespace corgi
