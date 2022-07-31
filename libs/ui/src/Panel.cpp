#include <corgi/rendering/renderer.h>
#include <corgi/rendering/texture.h>
#include <corgi/ui/Panel.h>
#include <corgi/utils/ResourcesCache.h>

namespace corgi::ui
{
void Panel::paint(Renderer& renderer)
{
    DrawList::NineSlice ns;

    ns.width  = width();
    ns.height = height();

    ns.x = real_x();
    ns.y = real_y();

    //try
    //{
    ns.material = *ResourcesCache::get<Material>(
        "corgi/materials/unlit/unlit_texture.mat");
    if(ns.material._texture_uniforms.empty())
        ns.material.add_texture(
            *ResourcesCache::get<Texture>(
                "corgi/textures/mp.tex"));
    else
        ns.material.set_texture(
            0, *ResourcesCache::get<Texture>(
                "corgi/textures/mp.tex"));
    //}
    //catch(const std::exception& e)
    //{
    //	//log_error("Could not pain ui::Panel  "+std::string(e.what()));
    //}

    auto texture = ns.material._texture_uniforms.begin()->texture;

    ns.material.enable_depth_test(false);

    const auto texture_width = static_cast<float>(texture->width());

    ns.top_border_    = 30.0f;
    ns.bottom_border_ = 30;
    ns.left_border_   = 30.0f;
    ns.right_border_  = 30;

    ns.left_border_uv_   = ns.left_border_ / texture_width;
    ns.right_border_uv_  = ns.right_border_ / texture_width;
    ns.top_border_uv_    = ns.top_border_ / texture_width;
    ns.bottom_border_uv_ = ns.bottom_border_ / texture_width;

    renderer.window_draw_list().add_nine_slice(ns);
}

void Panel::update(float elapsed_time)
{}
}    // namespace corgi::ui
