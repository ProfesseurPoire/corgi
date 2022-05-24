#pragma once

#include <corgi/components/RendererComponent.h>
#include <corgi/ecs/RefEntity.h>

namespace corgi
{
    class Texture;
    class Mesh;

    class MeshRenderer : public RendererComponent
    {
    public:
        /*
         * @brief I'm not too sure why I need the id for the meshRenderer
         */
        MeshRenderer(RefEntity id);
        MeshRenderer(RefEntity id, Material material);

        MeshRenderer(const MeshRenderer& other);
        MeshRenderer(MeshRenderer&& other) noexcept;

        MeshRenderer& operator=(const MeshRenderer& other);
        MeshRenderer& operator=(MeshRenderer&& other) noexcept;

        static inline std::vector<long long> layers;

        RefEntity _entity_id;
    };
}    // namespace corgi
