#pragma once

#include <corgi/ecs/System.h>
#include <corgi/ecs/Scene.h>

namespace corgi
{
    class Mesh;

    // Check the collisions for every collider in the game, and fire their callbacks
    // functions
    class SpriteRendererSystem : public AbstractSystem
    {
    public:
        static Mesh& get_sprite_mesh(unsigned int window_id);
        static Mesh& getUiMesh(unsigned int window_id);

        static void release_sprite_mesh();
        static void initializeSpriteMesh();
        static void initializeUiMesh();

        SpriteRendererSystem(Scene& scene);

        void before_update(float elapsed_time) override;

        Scene& _scene;
    };
}
