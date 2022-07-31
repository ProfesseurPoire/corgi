#pragma once

#include <corgi/ecs/Component.h>
#include <corgi/math/Vec3.h>

#include <memory>
#include <vector>

namespace corgi
{
class Entity;
class Mesh;

class ColliderComponent : public Component
{
    friend class CollisionSystem;
    friend class Renderer;
    friend class Physic;

    class CollisionEvent
    {
    public:
        void operator+=(
            std::function<void(Entity&, Entity&, ColliderComponent&, ColliderComponent&)>
                callback)
        {
            _callbacks.push_back(callback);
        }

        void operator-=(int index) { _callbacks.erase(_callbacks.begin() + index); }

        void clear() { _callbacks.clear(); }

        template<class... T>
        void invoke(T&&... args)
        {
            for(auto& callback : _callbacks)
                callback(args...);
        }

        std::vector<
            std::function<void(Entity&, Entity&, ColliderComponent&, ColliderComponent&)>>
            _callbacks;
    };

public:
    // Lifecycle

    ColliderComponent() = default;
    ColliderComponent(int layer);

    // Functions

    void              layer(int layer_id);
    [[nodiscard]] int layer() const;

    // Variables

    CollisionEvent on_enter;        // 16
    CollisionEvent on_exit;         // 16
    CollisionEvent on_collision;    // 16
    //
    //Store the normals of every triangle of the mesh
    std::vector<Vec3> normals;    // 12
    //
    // TODO : I can probably just use a pointer here
    std::shared_ptr<Mesh> _mesh;    // 8

    char layer_    = 0;        // 1
    bool colliding = false;    // 1

protected:
    bool collide(const Entity& a, const Entity& b, ColliderComponent* col);
};
}    // namespace corgi
