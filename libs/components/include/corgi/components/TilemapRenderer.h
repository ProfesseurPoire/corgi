#pragma once

#include <corgi/ecs/Component.h>
#include <corgi/ecs/RefEntity.h>
#include <corgi/resources/Tilemap.h>

namespace corgi
{
    class Texture;
    class Renderer;

    class TilemapRenderer : public Component
    {
    public:
        friend class Renderer;

        // Constructors

        TilemapRenderer();

        // Functions

        void initialize(Tilemap tilemap, RefEntity e);
        // tile_width and tile_height is set in pixel here

        // For now I send the renderer like that because I don't really know
        // how to do it otherwise
        //void set_tileset(Texture* tileset, int tile_width, int tile_height);

        // I mean, I'd need to make a "TilemapLayerRenderer" component
        // and have them all inherit from this?
        void generate_mesh(Tilemap tilemap, Tilemap::TileLayer layer);

        void generate_layer(RefEntity e, const Tilemap::TileLayer& tile_layer);

        void generate_tileset_layer(RefEntity                   parent,
                                    const Tilemap::TileLayer&   layer,
                                    const Tilemap::TilesetInfo& tileset_info);

        /*!
		 * @brief The goal of this function is simply to return the
		 *		  gid of the tile's tileset
		 */
        int extract_tileset_from_tileid(long long tile_id);

        // Variables

        int cameraLayer_ = 1;

        int _width {0};     // 4
        int _height {0};    // 4

        int _x {0};    // 4
        int _y {0};    // 4

        int _tile_width {0};     // 4 bytes
        int _tile_height {0};    // 4 bytes

        float scaling = 1.0f;    // 4 bytes
        float depth   = 0.0f;    // 4 bytes

        float    _opacity;       // 4 bytes
        unsigned buffer {0u};    // 4 bytes

        int points {0};    // 4 bytes

        Tilemap tilemap_;

        Tilemap::TileLayer layer_;

    private:
        // Functions

        void create_animated_tile(long long                      tile_id,
                                  Texture*                       texture,
                                  float                          left,
                                  float                          bottom,
                                  unsigned                       tileset_tile_width,
                                  unsigned                       tileset_tile_height,
                                  RefEntity                      e,
                                  Vector<Tilemap::TileAnimation> animations);
    };
}    // namespace corgi