#pragma once

#include <corgi/ecs/ComponentPools.h>
#include <corgi/ecs/EntityId.h>
#include <corgi/ecs/RefEntity.h>
#include <corgi/ecs/System.h>

#include <deque>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace corgi
{
class Entity;

/*!
	 * @brief	The goal of the scene is to glue the different ECS elements together.
	 *
	 *			Contains entities, components and systems that are supposed to interact
	 *			with each other
	 */
class Scene
{
public:
    // Constructors

    Scene();
    virtual ~Scene();

    // Functions

    template<class T, class... Args>
    void emplace_system(Args&&... args)
    {
        systems_order_.emplace_back(typeid(T));
        systems_.emplace(typeid(T), std::make_unique<T>(std::forward<Args>(args)...));
    }

    template<class T>
    [[nodiscard]] T* get_system()
    {

        if(!systems_.contains(typeid(T)))
        {
            return nullptr;
        }

        return dynamic_cast<T*>(systems_.at(typeid(T)).get());
    }

    template<class T>
    [[nodiscard]] bool has_system() const noexcept
    {
        return systems_.contains(typeid(T));
    }

    void unregister_entity_from_component_pools(EntityId id);

    /*Entity* find_by_name(std::string name);
		Entity* find_by_tag(std::string tag);*/

    void before_update(float elapsed_time);
    void update(float elapsed_time);
    void after_update(float elapsed_time);

    /*!
	 * @brief	This enum can be used to specify additional information on how to create
	 *			a new entity 
	 */
    enum class EntityCreationFlag
    {
        Default,
        NoTransform
    };

    /*!
	 * @brief	By default a transform component is automatically added to the entity
	 *			Use the EntityCreationFlag::NoTransform to creates one without transform
	 */
    RefEntity new_entity(const std::string& name = "NewEntity",
                         EntityCreationFlag      = EntityCreationFlag::Default);

    /*!
	 * @brief	Creates a copy of the given entity and attach it to the scene
	 */
    RefEntity new_entity(const Entity& entity);

    /*!
	 * @brief	Appends a new Entity to the scene and returns a new reference to it
	 */
    RefEntity append(const std::string& name = "NewEntity");

    /*!
	 * @brief	Returns a reference to the Entity with the given id
	 *
	 *			Throws an out of range exception if we can't find an entity with the given id
	 *
	 *			Mostly used by RefEntity because you shouldn't work with entity references
	 *			Use RefEntity objects instead
	 */
    Entity& get_entity(EntityId id);

    [[nodiscard]] const ComponentPools& component_maps() const { return _component_maps; }

    ComponentPools& component_maps() { return _component_maps; }

    RefEntity new_entity(Scene& scene, const std::string& name);
    RefEntity new_entity(RefEntity parent, const std::string& name);

    /*!
	 * @brief	Clone the current entity
	 *
	 *			If no @a parent is specified, the clone will be a sibling
	 *			of the current entity. Otherwise it'll be a child
	 *			of @a parent
	 */
    RefEntity clone(RefEntity base, RefEntity parent = RefEntity());

    /*!
	 * @brief	Removes every entity attached to the scene
	 */
    void clear();

    /*!
	 * @brief	Tries to find an entity called "name" inside the scene
	 *			Returns a pointer to the entity if founded, returns nullptr 
	 *			otherwise
	 */
    RefEntity find(std::string_view name);

    corgi::RefEntity root() { return root_; }

    void remove_entity(RefEntity entity);

    // There will be a copy anyways
    /*void add_canvas(const Canvas& canvas);
		void add_canvas(Canvas&& canvas);*/
    //Canvas& new_canvas();

    std::map<std::type_index, std::unique_ptr<AbstractSystem>>& systems();

    [[nodiscard]] std::vector<Entity>& entity_contiguous()
    {
        return _entities_contiguous;
    }

private:
    void     grow_ids();
    EntityId get_next_id();

    ComponentPools _component_maps;                                         // 24 bytes
    std::map<std::type_index, std::unique_ptr<AbstractSystem>> systems_;    // 24 bytes
    std::vector<std::type_index> systems_order_;                            // 32 bytes
    std::vector<Entity>          _entities_contiguous;                      // 32 bytes

    std::deque<EntityId> _usable_ids;    // 40 bytes
    corgi::RefEntity     root_;          // 16 bytes

    int _existing_id_count {0};    // 4 bytes

    char padding_[4];

    // 184 bytes
};
}    // namespace corgi
