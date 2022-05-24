#include <corgi/components/TextRenderer.h>
#include <corgi/components/Transform.h>
#include <corgi/ecs/Entity.h>
#include <corgi/rendering/Material.h>
#include <corgi/rendering/texture.h>
#include <corgi/resources/Font.h>
#include <corgi/resources/Mesh.h>
#include <corgi/utils/ResourcesCache.h>

namespace corgi
{
    static int ttt = corgi::Component::register_type_name(typeid(corgi::TextRenderer),
                                                          "Text Renderer");

    TextRenderer::TextRenderer(RefEntity entity)
    {
        _mesh_renderer = entity->add_component<MeshRenderer>(entity);

        _mesh_renderer->material =
            *ResourcesCache::get<Material>("corgi/materials/unlit/unlit_texture.mat");
        _mesh_renderer->_mesh = Mesh::new_standard_mesh();
        material().enable_depth_test(true);
        material().is_transparent(true);
    }

    TextRenderer::TextRenderer(const TextRenderer& other)
    {
        _mesh_renderer = other._mesh_renderer;
        font           = other.font;

        text    = other.text;
        scaling = other.scaling;
        offset  = other.offset;

        alignment = other.alignment;
    }

    TextRenderer::TextRenderer(TextRenderer&& other) noexcept
    {
        _mesh_renderer = other._mesh_renderer;
        font           = other.font;

        text      = other.text;
        scaling   = other.scaling;
        offset    = other.offset;
        alignment = other.alignment;
    }

    TextRenderer& TextRenderer::operator=(const TextRenderer& other)
    {
        _mesh_renderer = other._mesh_renderer;
        font           = other.font;

        text    = other.text;
        scaling = other.scaling;
        offset  = other.offset;

        alignment = other.alignment;

        return *this;
    }

    TextRenderer& TextRenderer::operator=(TextRenderer&& other) noexcept
    {
        _mesh_renderer = other._mesh_renderer;
        font           = other.font;

        text    = other.text;
        scaling = other.scaling;
        offset  = other.offset;

        alignment = other.alignment;
        return *this;
    }

    void corgi::TextRenderer::update_mesh()
    {
        // if(font != nullptr)
        // {
        //     if(material()._texture_uniforms.empty())
        //         material().add_texture(*font->texture());
        //     else
        //         material().set_texture(0, *font->texture());

        //     int i                 = 0;
        //     _mesh_renderer->_mesh = Mesh::new_standard_mesh();

        //     mesh()->vertices().resize(8 * 4 * (text.size()), 0.0f);

        //     for(auto c : text)
        //     {
        //         if(c != ' ')
        //         {
        //             mesh()->addTriangle(i, i + 1, i + 2);
        //             mesh()->addTriangle(i, i + 2, i + 3);
        //             i += 4;
        //         }
        //     }

        //     mesh()->update_vertices();

        //     i              = 0;
        //     auto& vertices = mesh()->vertices();

        //     float offs = 0;

        //     for(auto c : text)
        //     {
        //         if(c == ' ')
        //         {
        //             offs += 20;
        //             i++;
        //             continue;
        //         }

        //         const auto character_info = font->characters()[static_cast<unsigned int>(c)];
        //         const auto texture_width  = static_cast<float>(font->texture()->width());
        //         const auto texture_height = static_cast<float>(font->texture()->height());

        //         float v  = (texture_height - character_info.bh) / texture_height;
        //         float aa = character_info.bw / texture_width;

        //         const float glyph_x_offset = character_info.bl * scaling;
        //         const float glpyh_y_offset = -(character_info.bh * scaling - character_info.bt * scaling);
        //         const float glyph_width    = character_info.bw * scaling;
        //         const float glyph_height   = character_info.bh * scaling;

        //         vertices[i * 32 + 0] = offset.x + offs + glyph_x_offset;
        //         vertices[i * 32 + 1] = offset.y + glpyh_y_offset;
        //         vertices[i * 32 + 2] = offset.z;
        //         vertices[i * 32 + 3] = character_info.tx;
        //         vertices[i * 32 + 4] = v;
        //         vertices[i * 32 + 5] = 0.0f;
        //         vertices[i * 32 + 6] = 0.0f;
        //         vertices[i * 32 + 7] = 1.0f;

        //         vertices[i * 32 + 8]  = offset.x + offs + glyph_x_offset + glyph_width;
        //         vertices[i * 32 + 9]  = offset.y + glpyh_y_offset;
        //         vertices[i * 32 + 10] = offset.z;
        //         vertices[i * 32 + 11] = character_info.tx + aa;
        //         vertices[i * 32 + 12] = v;
        //         vertices[i * 32 + 13] = 0.0f;
        //         vertices[i * 32 + 14] = 0.0f;
        //         vertices[i * 32 + 15] = 1.0f;

        //         vertices[i * 32 + 16] = offset.x + offs + glyph_x_offset + glyph_width;
        //         vertices[i * 32 + 17] = offset.y + glpyh_y_offset + glyph_height;
        //         vertices[i * 32 + 18] = offset.z;
        //         vertices[i * 32 + 19] = character_info.tx + aa;
        //         vertices[i * 32 + 20] = 1.0f;
        //         vertices[i * 32 + 21] = 0.0f;
        //         vertices[i * 32 + 22] = 0.0f;
        //         vertices[i * 32 + 23] = 1.0f;

        //         vertices[i * 32 + 24] = offset.x + offs + glyph_x_offset;
        //         vertices[i * 32 + 25] = offset.y + glpyh_y_offset + glyph_height;
        //         vertices[i * 32 + 26] = offset.z;
        //         vertices[i * 32 + 27] = character_info.tx;
        //         vertices[i * 32 + 28] = 1.0f;
        //         vertices[i * 32 + 29] = 0.0f;
        //         vertices[i * 32 + 30] = 0.0f;
        //         vertices[i * 32 + 31] = 1.0f;

        //         offs += character_info.ax * scaling;
        //         i++;
        //     }

        //     if(alignment == HorizontalAlignment::Centered)
        //     {
        //         const auto max = vertices.size() / 8u;

        //         for(auto i = 0u; i < max; ++i)
        //             vertices[i * 8u] -= offs / 2.0f;
        //     }
        //     mesh()->update_vertices_really();
        // }
    }

    Material& TextRenderer::material() { return _mesh_renderer->material; }

    SharedPtr<Mesh> TextRenderer::mesh() { return _mesh_renderer->_mesh; }
}    // namespace corgi
