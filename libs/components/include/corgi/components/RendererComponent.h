#pragma once

#include <corgi/ecs/Component.h>
#include <corgi/ecs/Entity.h>
#include <corgi/rendering/Material.h>

#include <memory>

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

    Material              material = Material("empty");
    std::shared_ptr<Mesh> _mesh;
    size_t                entity_id = EntityId::npos;
    long long             layer     = 1;
};
}    // namespace corgi
