#pragma once

#include <corgi/components/MeshRenderer.h>
#include <corgi/components/RendererComponent.h>
#include <corgi/components/TextRenderer.h>
#include <corgi/ecs/ComponentPool.h>
#include <corgi/ecs/System.h>
#include <corgi/main/Game.h>

#include <vector>

namespace corgi
{
class RenderingSystem : public AbstractSystem
{
public:
    std::vector<MeshRenderer*> renderer_components_;
    Scene*                     _scene {nullptr};

    ComponentPool<MeshRenderer>& _meshes;

    RenderingSystem(Scene& scene, ComponentPool<MeshRenderer>& meshes)
        : _scene(&scene)
        , _meshes(meshes)
    {
    }

    void update(float) override
    {
        //auto enab = Entity::_enables

        renderer_components_.clear();
        renderer_components_.resize(_meshes.size());

        unsigned ind = 0;

        const auto size2 = _meshes.size();

        for(size_t i = 0u; i < size2; ++i)
        {
            MeshRenderer* c = &_meshes.components().data()[i];

            auto entity_id = _meshes.entity_id_int(i);

            c->entity_id                = entity_id;
            renderer_components_[ind++] = c;
        }
    }
};
}    // namespace corgi
