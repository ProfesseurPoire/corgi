#pragma once

#include <corgi/ecs/Component.h>
#include <corgi/rendering/Material.h>

namespace corgi
{
    class Mesh;

    class RendererComponent : public Component
    {
    public:
        RendererComponent() = default;
        RendererComponent(Material* mat)
            : material(*mat)
        {
        }
        RendererComponent(Material mat)
            : material(mat)
        {
        }
        ~RendererComponent() override = default;

        Material        material = Material("empty");
        SharedPtr<Mesh> _mesh;
        int             entity_id = -1;
        long long       layer     = 1;
    };
}    // namespace corgi
