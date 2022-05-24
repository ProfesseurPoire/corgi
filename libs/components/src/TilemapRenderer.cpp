#include <corgi/components/Animator.h>
#include <corgi/components/MeshRenderer.h>
#include <corgi/components/SpriteRenderer.h>
#include <corgi/components/TilemapRenderer.h>
#include <corgi/components/Transform.h>
#include <corgi/ecs/Entity.h>
#include <corgi/filesystem/FileSystem.h>
#include <corgi/math/MathUtils.h>
#include <corgi/rendering/texture.h>
#include <corgi/resources/Mesh.h>
#include <corgi/utils/ResourcesCache.h>

#include <cassert>
#include <numbers>

namespace corgi
{
    // Tiled stores information about tile flipping on 3 bits of the id
    static const unsigned FLIPPED_HORIZONTALLY_FLAG = 0x80000000;
    static const unsigned FLIPPED_VERTICALLY_FLAG   = 0x40000000;
    static const unsigned FLIPPED_DIAGONALLY_FLAG   = 0x20000000;

    static Animation make_animation(Texture&                              texture,
                                    const Vector<Tilemap::TileAnimation>& animations,
                                    unsigned                              tile_width,
                                    unsigned                              tile_height)
    {
        Animation anim;
        anim.name("tile_animation");
        anim.looping   = true;
        anim.flipped_x = false;
        anim.frames.reserve(animations.size());

        for(const auto& tile_animation : animations)
        {
            const int cols = static_cast<int>(texture.width() / tile_width);

            const auto x_offset = tile_animation.tile_id % cols;
            const auto y_offset = tile_animation.tile_id / cols;

            Sprite sprite(tile_width, tile_height, x_offset * tile_width,
                          texture.height() - y_offset * tile_height - tile_height, 0.5f,
                          0.5f, &texture);

            anim.frames.emplace_back(sprite, tile_animation.duration);
        }
        return anim;
    }

    /*!
     *  Check if the given tile_id is animated in the given TilesetInfo
     */
    //static bool is_tile_animated(long long tile_id, const Tilemap::TilesetInfo& tileset_info)
    //{
    //	const auto result = std::ranges::find_if(tileset_info.tiles, [&](const Tilemap::TileInfo& ti)->bool
    //		{
    //			return ti.id == tile_id;
    //		});

    //	return result != tileset_info.tiles.end();
    //}

    void TilemapRenderer::create_animated_tile(long long tile_id,
                                               Texture*  texture,
                                               float     left,
                                               float     bottom,
                                               unsigned  tileset_tile_width,
                                               unsigned  tileset_tile_height,
                                               RefEntity e,
                                               Vector<Tilemap::TileAnimation> animations)
    {
        // What an actual fucking mess

        const bool flipped_horizontally = (tile_id & FLIPPED_HORIZONTALLY_FLAG);
        const bool flipped_vertically   = (tile_id & FLIPPED_VERTICALLY_FLAG);
        const bool flipped_diagonally   = (tile_id & FLIPPED_DIAGONALLY_FLAG);

        const auto pi =
            std::numbers::pi_v<float>;    // Kinda sucks that I have to do that but ok

        float rotation_value {0.0f};

        // This values are actually use to know if I must flip on the x or y axis "for real"
        // and not just rotate the thing
        bool flip_v {false};
        bool flip_h {false};

        if(!flipped_horizontally && flipped_vertically && flipped_diagonally)
            rotation_value = pi / 2.0f;

        if(flipped_horizontally && flipped_vertically && !flipped_diagonally)
            rotation_value = pi;

        if(flipped_horizontally && !flipped_vertically && flipped_diagonally)
            rotation_value = pi + pi / 2.0f;

        if(!flipped_horizontally && flipped_vertically && !flipped_diagonally)
            flip_v = true;

        if(flipped_horizontally && flipped_vertically && flipped_diagonally)
        {
            flip_v         = true;
            rotation_value = pi / 2.0f;
        }

        if(flipped_horizontally && !flipped_vertically && !flipped_diagonally)
            flip_h = true;

        if(!flipped_horizontally && !flipped_vertically && flipped_diagonally)
        {
            flip_h         = true;
            rotation_value = pi / 2.0f;
        }

        auto entity = e->emplace_back("RootTileAnimation");
        entity->add_component<Transform>(left + tileset_tile_width / 2.0f,
                                         bottom - tileset_tile_height / 2.0f);

        auto animation_entity = entity->emplace_back("TileAnimation");
        auto tr               = animation_entity->add_component<Transform>();

        tr->euler_angles(0.0f, 0.0f, rotation_value);

        // TODO : This might not always work, if something feels wrong check this

        if(flip_h)
            tr->euler_angles(0.0f, math::pi, rotation_value);

        if(flip_v)
            tr->euler_angles(math::pi, 0.0f, rotation_value);

        auto sr         = animation_entity->add_component<SpriteRenderer>();
        sr->cameraLayer = cameraLayer_;
        auto mat        = sr->material();

        // We know that the material we use for sprites contains
        // one sampler that is the main texture
        mat._texture_uniforms.at(0).texture = texture;

        sr->material(mat);

        auto animator = animation_entity->add_component<Animator>();
        animator->add_animation(make_animation(*texture, animations, tileset_tile_width,
                                               tileset_tile_height));
        animator->play("tile_animation");
    }

    void TilemapRenderer::generate_tileset_layer(RefEntity                   parent,
                                                 const Tilemap::TileLayer&   layer,
                                                 const Tilemap::TilesetInfo& tileset_info)
    {
        bool use_y_as_z   = layer.name == "Objects";
        bool always_front = layer.name == "AlwaysFront";

        // I generate the component needed to make the thing
        auto e = parent->emplace_back(layer.name.c_str());
        e->add_component<Transform>();
        auto mesh_renderer   = e->add_component<MeshRenderer>(e);
        mesh_renderer->layer = cameraLayer_;

        int actualLayer = mesh_renderer->layer;

        // Getting back the basic information from the tilemap and layer
        const auto width  = layer.width;
        const auto height = layer.height;
        const auto x      = layer.x;
        const auto y      = layer.y;

        const auto tilemap_tile_width  = tilemap_.tile_width;
        const auto tilemap_tile_height = tilemap_.tile_height;

        const auto tileset_tile_width  = tileset_info.tile_width;
        const auto tileset_tile_height = tileset_info.tile_height;

        // We get back the tileset's texture

        auto* texture = ResourcesCache::get<Texture>(tileset_info.image);

        // Define the material

        auto& material = mesh_renderer->material;

        material =
            *ResourcesCache::get<Material>("corgi/materials/unlit/unlit_texture.mat");

        if(material._texture_uniforms.empty())
            material.add_texture(*texture);
        else
            material.set_texture(0, *texture);

        material.enable_depth_test(true);
        material.depth_test = DepthTest::LEqual;
        material.set_uniform("use_flat_color", 0);

        //const int  ratio			= (texture->width() / _tile_width);
        const float tex_tile_width = static_cast<float>(tileset_info.tile_width) /
                                     static_cast<float>(texture->width());
        const float tex_tile_height = static_cast<float>(tileset_info.tile_height) /
                                      static_cast<float>(texture->height());

        auto mesh = Mesh::new_standard_mesh();

        int  tile_count        = 0;
        auto tileset_info_size = tileset_info.tiles.size();

        int w = static_cast<int>(width);

        // We first check how many tile we actually have to draw to properly size
        // the mesh that will be created
        for(int i = 0; i < height; ++i)
        {
            for(int j = 0; j < w; ++j)
            {
                // Get the tile id
                long long tile_id = layer.data[i * w + j];

                // Remove the flags from the id
                tile_id &= ~(FLIPPED_HORIZONTALLY_FLAG | FLIPPED_VERTICALLY_FLAG |
                             FLIPPED_DIAGONALLY_FLAG);

                // if the tile id is equal to zero, we just skip the thing
                if(tile_id == 0)
                    continue;

                auto tileset_gid = extract_tileset_from_tileid(tile_id);

                if(tileset_gid != tileset_info.firstgid)
                    continue;

                tile_id -= tileset_gid;

                // Look if the tile is animated

                bool     found       = false;
                unsigned found_index = -1;

                for(SizeType i = 0; i < tileset_info_size; i++)
                {
                    if(tileset_info.tiles[i].id == tile_id)
                    {
                        found       = true;
                        found_index = i;
                        break;
                    }
                }

                if(found)
                {
                    if(!tileset_info.tiles[found_index].animations.empty())
                        continue;
                }

                tile_count++;
            }
        }

        float* vertices = new float[tile_count * 4 * 8];
        auto*  indices  = new unsigned[tile_count * 3 * 2];

        int current_tile = 0;

        // Drawing the TilemapRenderer in the most naive way for now
        for(int i = 0; i < height; ++i)
        {
            float bottom = static_cast<float>(y) - i * tilemap_tile_height;

            for(int j = 0; j < w; ++j)
            {
                float left = static_cast<float>(x) + j * tilemap_tile_width;

                // Get the tile id
                long long tile_id = layer.data[i * w + j];

                const bool flipped_horizontally = (tile_id & FLIPPED_HORIZONTALLY_FLAG);
                const bool flipped_vertically   = (tile_id & FLIPPED_VERTICALLY_FLAG);
                const bool flipped_diagonally   = (tile_id & FLIPPED_DIAGONALLY_FLAG);

                // Remove the flags from the id
                tile_id &= ~(FLIPPED_HORIZONTALLY_FLAG | FLIPPED_VERTICALLY_FLAG |
                             FLIPPED_DIAGONALLY_FLAG);

                // long long tile_x = tile_id % ratio;
                // long long tile_y = tile_id / ratio;

                // if the tile id is equal to zero, we just skip the thing
                if(tile_id == 0)
                    continue;

                // now we find the real tileid, because somehow
                // the tileset id is into the tile_id, and we substract it
                // though to be fair, this won't really support more than
                // 1 tileset for now on

                // WARNING : This only works if we go from the highest
                // tileset first, so, if something goes wrong, maybe
                // check that out

                auto tileset_gid = extract_tileset_from_tileid(tile_id);

                if(tileset_gid != tileset_info.firstgid)
                    continue;

                tile_id -= tileset_gid;

                // Look if the tile is animated

                bool     found       = false;
                unsigned found_index = -1;

                for(SizeType i = 0; i < tileset_info_size; i++)
                {
                    if(tileset_info.tiles[i].id == tile_id)
                    {
                        found       = true;
                        found_index = i;
                        break;
                    }
                }

                if(found)
                {
                    if(!tileset_info.tiles[found_index].animations.empty())
                    {
                        create_animated_tile(tile_id, texture, left, bottom,
                                             tileset_tile_width, tileset_tile_height, e,
                                             tileset_info.tiles[found_index].animations);
                        // We move to the next tile
                        continue;
                    }
                }

                // Managing flips and rotations

                float      rotation_value {0.0f};
                const auto pi = std::numbers::pi_v<float>;

                bool flip_v {false};
                bool flip_h {false};

                if(!flipped_horizontally && flipped_vertically && flipped_diagonally)
                    rotation_value = pi / 2.0f;

                if(flipped_horizontally && flipped_vertically && !flipped_diagonally)
                    rotation_value = pi;

                if(flipped_horizontally && !flipped_vertically && flipped_diagonally)
                    rotation_value = pi + pi / 2.0f;

                if(!flipped_horizontally && flipped_vertically && !flipped_diagonally)
                    flip_v = true;

                if(flipped_horizontally && flipped_vertically && flipped_diagonally)
                {
                    flip_v         = true;
                    rotation_value = pi / 2.0f;
                }

                if(flipped_horizontally && !flipped_vertically && !flipped_diagonally)
                    flip_h = true;

                if(!flipped_horizontally && !flipped_vertically && flipped_diagonally)
                {
                    flip_h         = true;
                    rotation_value = pi / 2.0f;
                }

                Matrix mat = Matrix::rotation_z(rotation_value);

                // First step, tile, center, used with the rotation thing

                float z_value = 0.0f;

                if(use_y_as_z)
                    z_value = -(bottom - tileset_tile_height / 2.0f -
                                tileset_tile_height / 2.0f);

                const auto ttw = static_cast<float>(tileset_tile_width);
                const auto tth = static_cast<float>(tileset_tile_height);

                Vec3 center(left + ttw / 2.0f, bottom - tth / 2.0f, z_value);

                Vec3 v1(-ttw / 2.0f, tth / 2.0f, 0.0f);
                Vec3 v2(-ttw / 2.0f, -tth / 2.0f, 0.0f);
                Vec3 v3(ttw / 2.0f, -tth / 2.0f, 0.0f);
                Vec3 v4(ttw / 2.0f, tth / 2.0f, 0.0f);

                if(flip_v)
                {
                    v1 = Vec3(-ttw / 2.0f, -tth / 2.0f, 0.0f);
                    v2 = Vec3(-ttw / 2.0f, tth / 2.0f, 0.0f);
                    v3 = Vec3(ttw / 2.0f, tth / 2.0f, 0.0f);
                    v4 = Vec3(ttw / 2.0f, -tth / 2.0f, 0.0f);
                }

                if(flip_h)
                {
                    v1 = Vec3(ttw / 2.0f, tth / 2.0f, 0.0f);
                    v2 = Vec3(ttw / 2.0f, -tth / 2.0f, 0.0f);
                    v3 = Vec3(-ttw / 2.0f, -tth / 2.0f, 0.0f);
                    v4 = Vec3(-ttw / 2.0f, tth / 2.0f, 0.0f);
                }

                // After rotating a tile, we put it back where it's supposed to be

                auto r1 = mat * v1 + center;
                auto r2 = mat * v2 + center;
                auto r3 = mat * v3 + center;
                auto r4 = mat * v4 + center;

                const auto tile_width_ratio = ttw / static_cast<float>(texture->width());
                const auto tile_height_ratio =
                    tth / static_cast<float>(texture->height());

                unsigned cols = texture->width() / tileset_info.tile_width;

                long long xcoord = tile_id % cols;
                long long ycoord = tile_id / cols;

                Vec2 coord(xcoord * tile_width_ratio, 1.0f - ycoord * tile_height_ratio);

                const int vertex_per_tile       = 4;
                const int attributes_per_vertex = 8;
                const int sum                   = vertex_per_tile * attributes_per_vertex;

                vertices[current_tile * sum]     = r1.x;
                vertices[current_tile * sum + 1] = r1.y;
                vertices[current_tile * sum + 2] = r1.z;

                vertices[current_tile * sum + 3] = coord.x;
                vertices[current_tile * sum + 4] = coord.y;

                vertices[(current_tile)*sum + 8]  = r2.x;
                vertices[(current_tile)*sum + 9]  = r2.y;
                vertices[(current_tile)*sum + 10] = r2.z;

                vertices[(current_tile)*sum + 11] = coord.x;
                vertices[(current_tile)*sum + 12] = coord.y - tex_tile_height;

                vertices[(current_tile)*sum + 16] = r3.x;
                vertices[(current_tile)*sum + 17] = r3.y;
                vertices[(current_tile)*sum + 18] = r3.z;

                vertices[(current_tile)*sum + 19] = coord.x + tex_tile_width;
                vertices[(current_tile)*sum + 20] = coord.y - tex_tile_height;

                vertices[(current_tile)*sum + 24] = r4.x;
                vertices[(current_tile)*sum + 25] = r4.y;
                vertices[(current_tile)*sum + 26] = r4.z;

                vertices[(current_tile)*sum + 27] = coord.x + tex_tile_width;
                vertices[(current_tile)*sum + 28] = coord.y;

                int in = (current_tile + 1) * 4 - 4;

                indices[current_tile * 6]     = in;
                indices[current_tile * 6 + 1] = in + 1;
                indices[current_tile * 6 + 2] = in + 2;

                indices[current_tile * 6 + 3] = in;
                indices[current_tile * 6 + 4] = in + 2;
                indices[current_tile * 6 + 5] = in + 3;

                current_tile++;
            }
        }

        mesh->set_vertices(vertices, tile_count * 8 * 4);
        mesh->set_indices(indices, tile_count * 6);

        if(always_front)
        {
            mesh_renderer->material.enable_depth_test(false);
            mesh_renderer->material.render_queue = 10000;
        }

        mesh->update_vertices();
        //mesh_renderer->material.polygon_mode(Material::PolygonMode::Line);
        mesh_renderer->_mesh = mesh;
        mesh->build_bounding_volumes();
    }

    int TilemapRenderer::extract_tileset_from_tileid(long long tile_id)
    {
        for(unsigned i = tilemap_.tileset_infos.size() - 1; i > 0; i--)
        {
            const auto& tileset_info = tilemap_.tileset_infos[i];

            if(tileset_info.firstgid <= tile_id)
                return tileset_info.firstgid;
        }

        const auto& tileset_info = tilemap_.tileset_infos[0];

        if(tileset_info.firstgid <= tile_id)
            return tileset_info.firstgid;
        // TODO not to sure about that lol
        return -1;
    }

    void TilemapRenderer::generate_layer(RefEntity                 e,
                                         const Tilemap::TileLayer& tile_layer)
    {
        auto entity = e->emplace_back(tile_layer.name.c_str());
        entity->add_component<Transform>();
        entity->get_component<Transform>()->position(0.0f, 0.0f, depth);

        for(const auto& tileset_info : tilemap_.tileset_infos)
            generate_tileset_layer(entity, tile_layer, tileset_info);
    }

    TilemapRenderer::TilemapRenderer() {}

    void TilemapRenderer::initialize(Tilemap tilemap, RefEntity e)
    {
        tilemap_ = std::move(tilemap);
        depth    = 0.0f;

        for(const auto& layer : tilemap_.tile_layers)
        {
            generate_layer(e, layer);
            depth += 0.1f;
        }
    }

    void TilemapRenderer::generate_mesh(Tilemap tilemap, Tilemap::TileLayer layer)
    {
        _height = static_cast<int>(layer.height);
        _width  = static_cast<int>(layer.width);

        layer_   = layer;
        tilemap_ = std::move(tilemap);
    }
}    // namespace corgi
