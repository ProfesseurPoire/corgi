#include <corgi/components/Transform.h>
#include <corgi/ecs/Entity.h>
#include <corgi/ecs/Scene.h>
#include <corgi/systems/TransformSystem.h>

#include <algorithm>
#include <map>
#include <unordered_map>

namespace corgi
{
static Matrix local_matrix(Transform& transform)
{
    return Matrix::translation(transform.position().x, transform.position().y,
                               transform.position().z) *
           Matrix::rotation_x(transform.euler_angles().x) *
           Matrix::rotation_y(transform.euler_angles().y) *
           Matrix::rotation_z(transform.euler_angles().z) *
           Matrix::scale(transform.scale().x, transform.scale().y, transform.scale().z);
}

TransformSystem::TransformSystem(Scene& scene, ComponentPool<Transform>& transforms)
    : transforms_(transforms)
    , _scene(scene)
{
}

void TransformSystem::update_component(Transform& transform, Entity& entity)
{
    if(transform._dirty)    // Means the transform needs to be updated
    {
        if(!transform.is_world())
        {
            if(entity.parent())
                transform._world_matrix =
                    transforms_.get(entity.parent()->id())._world_matrix *
                    local_matrix(transform);
            else
                transform._world_matrix = local_matrix(transform);
        }
        else
        {
            transform._world_matrix = local_matrix(transform);
        }

        transform._dirty         = false;
        transform._inverse_dirty = false;

        // Because the children transformations depends on their parent, we have to
        // update them. Their dirty flag will set to false so we don't update them more than once
        for(auto& child : entity.children())
        {
            if(child->has_component<Transform>())
            {
                child->get_component<Transform>()->_dirty         = true;
                child->get_component<Transform>()->_inverse_dirty = true;
            }
        }
    }
}

void TransformSystem::update(float elapsed_time)
{
    std::vector<int> indexes(transforms_.components().size());

    // So the general idea is that I want to sort the transforms by
    // their depth
    // So I start by looping through every transform, looking for their Entity,
    // Calling the depth function

    auto* transforms              = transforms_.components().data();
    auto* comp_index_to_entity_id = transforms_.component_index_to_entity_id().data();
    auto* entity_id_to_comp       = transforms_._entity_id_to_components_vector.data();
    auto* entity_vector           = _scene.entity_contiguous().data();

    const auto size = transforms_.size();

    auto entity_size = sizeof(Entity);

    for(int i = 0; i < size; ++i)
    {
        auto eid             = comp_index_to_entity_id[i];
        transforms[i]._depth = _scene.entity_contiguous()[eid]._depth;
    }

    for(int i = 0; i < size; ++i)
        indexes[i] = i;

    // This might looks weird but it actually speeds up the first sort
    // at least in debug mode by a huge margin
    const auto comps = transforms_.components().data();

    // Then there is this sort that kinda sucks
    std::sort(indexes.begin(), indexes.end(),
              [&](int& a, int& b) { return comps[a]._depth < comps[b]._depth; });

    // TODO : Keep track of the depth without use of recursive function
    std::sort(transforms_.components().begin(), transforms_.components().end(),
              [&](Transform& a, Transform& b) { return (a._depth < b._depth); });

    // Not a huge fan of copying this map here but well, without doing that
    // I won't know the correct entity id

    auto copy_map = transforms_.component_index_to_entity_id();

    // Updating the maps of the ComponentMap after sorting
    for(int i = 0; i < size; ++i)
    {
        auto eid                   = copy_map[indexes[i]];
        comp_index_to_entity_id[i] = eid;
        entity_id_to_comp[eid]     = i;
    }

    for(int i = 0; i < size; ++i)
    {
        auto e = comp_index_to_entity_id[i];
        update_component(transforms[i], entity_vector[e]);
    }
}
}    // namespace corgi
