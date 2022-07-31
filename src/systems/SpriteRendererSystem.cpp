#include "corgi/ecs/Entity.h"

#include <corgi/components/MeshRenderer.h>
#include <corgi/components/SpriteRenderer.h>
#include <corgi/ecs/RefEntity.h>
#include <corgi/main/Game.h>
#include <corgi/rendering/texture.h>
#include <corgi/resources/Mesh.h>
#include <corgi/systems/SpriteRendererSystem.h>

#include <memory>

namespace corgi
{
// All sprite renderer use this mesh
// This mesh is deleted in the Game destructor
static Material default_material;

static std::string main_texture      = "main_texture";
static std::string sprite_uniform    = "sprite";
static std::string spritetex_uniform = "sprite_tex";
static std::string flip_x            = "flip_x";

std::shared_ptr<Mesh> sprite_meshes;
std::shared_ptr<Mesh> sUiMeshes;

Mesh& SpriteRendererSystem::get_sprite_mesh(unsigned int window_id)
{
    if(!sprite_meshes)
    {
        std::vector<float> vertices({-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, -1.0f,
                                     0.0f,  1.0f,  0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
                                     1.0f,  -1.0f, 1.0f, 0.0f, 0.0f, 1.0f});

        std::vector<unsigned> indexes({0, 1, 2, 0, 2, 3});

        auto sprite_mesh =
            Mesh::new_standard_2D_mesh(std::move(vertices), std::move(indexes));

        sprite_mesh->build_bounding_volumes();
    }
    return *sprite_meshes;
}

Mesh& SpriteRendererSystem::getUiMesh(unsigned int window_id)
{
    if(!sUiMeshes)
    {
        // Inverted uv for ui
        std::vector<float> vertices({0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
                                     0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
                                     0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f});

        std::vector<unsigned> indexes({0, 1, 2, 0, 2, 3});

        sUiMeshes = Mesh::new_standard_2D_mesh(std::move(vertices), std::move(indexes));
        sUiMeshes->build_bounding_volumes();
    }
    return *sUiMeshes.get();
}

void SpriteRendererSystem::release_sprite_mesh()
{
    //sprite_mesh.reset();
    //sUiMesh.reset();
}

SpriteRendererSystem::SpriteRendererSystem(Scene& scene)
    : _scene(scene)
{
}

void SpriteRendererSystem::initializeUiMesh() {}

void SpriteRendererSystem::initializeSpriteMesh()
{
    /*initializeUiMesh();

        if(!sprite_mesh)
        {
            std::vector<float> vertices({-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, -1.0f,
                                         0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
                                         1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f});

            std::vector<unsigned> indexes({0, 1, 2, 0, 2, 3});

            sprite_mesh =
                Mesh::new_standard_2D_mesh(std::move(vertices), std::move(indexes));
            sprite_mesh->build_bounding_volumes();
        }*/
}

void SpriteRendererSystem::before_update(float elapsed_time)
{
    auto* sprites        = _scene.component_maps().get<SpriteRenderer>();
    auto* mesh_renderers = _scene.component_maps().get<MeshRenderer>();

    auto* animator_array = sprites->components().data();
    auto  size           = sprites->components().size();

    for(auto i = 0u; i < size; i++)
    {
        auto&       sprite_renderer = animator_array[i];
        const auto& sprite          = sprite_renderer.sprite_;

        if(!sprite_renderer._dirty)
            continue;

        const auto entity_id = EntityId(sprites->component_index_to_entity_id().at(i));

        auto& entity =
            _scene.entity_contiguous().at(sprites->component_index_to_entity_id().at(i));

        if(!mesh_renderers->contains(entity_id))
            mesh_renderers->add_param(entity_id, RefEntity(entity.scene(), entity));

        if(!entity.is_enabled())
            continue;

        auto& mesh_renderer = mesh_renderers->get(entity_id);

        mesh_renderer.material = sprite_renderer._material;
        mesh_renderer._mesh    = sprite_meshes;
        mesh_renderer.layer    = sprite_renderer.cameraLayer;

        mesh_renderer.material.enable_depth_test(true);
        mesh_renderer.material.is_transparent(true);

        mesh_renderer.material.set_uniform("flat_color", Vec4(2.0f, 2.0f, 2.0f, 1.0f));
        mesh_renderer.material.set_uniform("use_flat_color", 0);

        mesh_renderer.material.set_uniform("flip_x", sprite_renderer._flipped_x);

        if(mesh_renderer.material._texture_uniforms.empty())
            mesh_renderer.material.add_texture(*sprite_renderer.sprite_.texture,
                                               main_texture.c_str());
        else
            mesh_renderer.material.set_texture(0, *sprite_renderer.sprite_.texture);

        mesh_renderer.material.set_uniform("pivot_x", sprite.pivot_value.x);
        mesh_renderer.material.set_uniform("pivot_y", sprite.pivot_value.y);

        mesh_renderer.material.set_uniform("sprite_width", sprite.width);
        mesh_renderer.material.set_uniform("sprite_height", sprite.height);

        if(sprite_renderer.sprite_.texture != nullptr)
        {
            mesh_renderer.material.set_uniform("offset_x",
                                               sprite_renderer.sprite_.offset_x);
            mesh_renderer.material.set_uniform("offset_y",
                                               sprite_renderer.sprite_.offset_y);

            mesh_renderer.material.set_uniform("texture_width",
                                               sprite_renderer.sprite_.texture->width());
            mesh_renderer.material.set_uniform("texture_height",
                                               sprite_renderer.sprite_.texture->height());
        }
    }
}
}    // namespace corgi
