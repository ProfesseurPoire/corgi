#pragma once

#include <corgi/ecs/ComponentPool.h>
#include <corgi/ecs/EntityId.h>
#include <corgi/ecs/RefEntity.h>
#include <corgi/ecs/Scene.h>

#include <typeindex>
#include <vector>

namespace corgi
{
/*
	 * @brief	An entity is basically just an ID that components attaches too
	 *			Whenever a new Entity is created, it is given a unique identifier (up to 2 billions)
	 */
class Entity
{
    friend class Scene;
    friend class Game;
    friend class Renderer;
    friend class Physic;

public:
    enum class IteratorMode : char
    {
        DepthFirst,
        BreadthFirst
    };

    class Iterator
    {
    public:
        using iterator_category = std::input_iterator_tag;
        using value_type        = RefEntity;
        using difference_type   = int;
        using pointer           = RefEntity;
        using reference         = RefEntity;

        Iterator() = default;

        explicit Iterator(Entity& node, IteratorMode mode = IteratorMode::DepthFirst);

        [[nodiscard]] RefEntity operator*() const noexcept { return current_node_; }
        [[nodiscard]] RefEntity operator->() { return current_node_; }
        [[nodiscard]] RefEntity get();

        void increment_breadth_first();
        void increment_depth_first();

        Iterator& operator++();

        [[nodiscard]] bool operator!=(const Iterator& iterator) const noexcept;
        [[nodiscard]] bool operator==(const Iterator& iterator) const;

    private:
        std::deque<RefEntity> queue_;
        IteratorMode          mode_ {IteratorMode::DepthFirst};
        bool                  recursive_ {true};
        RefEntity             current_node_;
    };

    // Lifecycle

    Entity() = default;
    Entity(EntityId id, Scene& scene, const char* name = "");

    /*!
		 * @brief	Entity without a scene can only be built by another already
		 *			attached entity
		 */
    Entity(EntityId id, RefEntity parent, const char* name = "NewEntity");

    ~Entity();

    // TODO : Check this, probably wrong
    Entity(Entity&& entity) noexcept;
    Entity(const Entity& entity);

    Entity& operator=(const Entity& entity);
    Entity& operator=(Entity&& entity) noexcept;

    // Accessors

    /*!
		 * @brief	Returns the entity's unique identifier
		 */
    [[nodiscard]] EntityId id() const noexcept;

    /*!
		 * @brief	Returns the entity's name
		 */
    [[nodiscard]] const char* name() const;

    /*!
		 * @brief	Returns true if the entity is currently enabled
		 *			A disabled entity will not update its components and ignore by the systems
		 */
    [[nodiscard]] bool is_enabled() const noexcept;

    // Lookup

    /*!
		 * @brief	Tries to find an entity called @a name in the entity's children
		 *
		 *			This functions is recursive, meaning it'll also checks inside
		 *			the children's children
		 *
		 * @param	name Name of the entity we try to find
		 *
		 * @return	std::optional holds std::nullopt if no entity could be found. Otherwise,
		 *			returns a std::reference_wrapper to our Entity
		 */
    [[nodiscard]] RefEntity find(const char* name) noexcept;

    // Functions

    /*!
		 * @brief	Returns the entity's parent. If the entity has no parent, returns nullptr instead
		 */
    [[nodiscard]] RefEntity       parent() noexcept;
    [[nodiscard]] const RefEntity parent() const noexcept;

    [[nodiscard]] Scene& scene() noexcept;

    void parent(RefEntity new_parent);

    void clear() noexcept;

    /*!
		 * @brief	Finds the depth of the current entity
		 */
    [[nodiscard]] int depth(int d = 0) const noexcept;

    /*!
		 * @brief	The entity will remove itself from its parent, thus deleting it
		 *
		 *			I'm really not sure this is actually a correct way to do this,
		 *			since technically at the end of the function, the entity doesn't
		 *			exist anymore but well
		 */
    void remove();

    /*!
		 * @brief	Tries to remove the entity passed as a parameter from the current entity children list
		 */
    void remove_child(RefEntity e);

    /*!
		 * @brief Changes the name of the character
		 */
    void rename(const char* n);

    // TODO : not sure this is actually really use atm, since it's mostly colliders that uses a layer
    [[nodiscard]] long long current_layer() const;
    void                    current_layer(int cl);

    long long actual_layer();

    /*!
		 * @brief	Enables the entity its children
		 *
		 *			Disabled entities won't usually update their components (Depends on
		 *			the system implementation)
		 */
    void enable();

    /*!
		 * @brief	Disables the entity and its children
		 *
		 *			Disabled entities won't usually update their components (Depends on
		 *			the system implementation)
		 */
    void disable();

    /*!
		 * @brief	Returns the entity's children list as a const reference
		 */
    [[nodiscard]] const std::vector<RefEntity>& children() const noexcept;

    /*!
		 * @brief	Returns the entity's children list as a reference
		 */
    [[nodiscard]] std::vector<RefEntity>& children() noexcept;

    RefEntity emplace_back(const char* name);

    [[nodiscard]] Iterator begin() { return Iterator(*this); }
    [[nodiscard]] Iterator end() { return Iterator(); }

    void remove_component_of_type(std::type_index index);

    template<class T>
    void remove_component()
    {
        remove_component_of_type(typeid(T));
    }

    template<class T, class... Args>
    Ref<T> add_component(Args&&... args)
    {
        if(!scene_->component_maps().contains<T>())
            scene_->component_maps().add<T>();

        return scene_->component_maps().get<T>()->add_param(_id,
                                                            std::forward<Args>(args)...);
    }

    /*
		 * @brief	Checks if the entity stores a component of the given
		 *			template parameter type
		 *
		 * @return	Returns true if the component is stored, false otherwise
		 */
    template<class T>
    [[nodiscard]] bool has_component() const
    {
        return has_component(typeid(T));
    }

    [[nodiscard]] bool has_component(std::type_index t) const noexcept;

    template<class T>
    [[nodiscard]] Ref<T> get_component()
    {
        // We first check if there's a pool that could hold the component
        if(!scene_->component_maps().contains<T>())
        {
            std::terminate();
            //if(Component::type_name.contains(typeid(T)))
            //	throw std::invalid_argument("No component Pool of type " +Component::type_name.at(typeid(T))+ " could be found in entity \""+name_+"\"");
            //throw std::invalid_argument("No component pool of unknown type could be found in entity \""+name_+"\"");
        }

        if(!scene_->component_maps().get<T>()->contains(_id))
        {
            std::terminate();
            //throw;
            //if(Component::type_name.contains(typeid(T)))
            //	throw std::invalid_argument("No component of type " +Component::type_name.at(typeid(T))+ " could be found in entity \""+name_+"\"");
            //throw std::invalid_argument("No component of unknown type could be found in entity "+name_+"\"");
        }
        return scene_->component_maps().get<T>()->get_ref(_id);
    }

    template<class T>
    [[nodiscard]] ConstRef<T> get_component() const
    {
        // We first check if there's a pool that could hold the component
        if(!scene_->component_maps().contains<T>())
        {
            //if(Component::type_name.contains(typeid(T)))
            //	throw std::invalid_argument("No component Pool of type " +Component::type_name.at(typeid(T))+ " could be found in entity \""+name_+"\"");
            //throw std::invalid_argument("No component pool of unknown type could be found in entity \""+name_+"\"");
        }

        if(!scene_->component_maps().get<T>()->contains(_id))
        {
            //if(Component::type_name.contains(typeid(T)))
            //	throw std::invalid_argument("No component of type " +Component::type_name.at(typeid(T))+ " could be found in entity \""+name_+"\"");
            //throw std::invalid_argument("No component of unknown type could be found in entity "+name_+"\"");
        }
        return scene_->component_maps().get<T>()->get_const_ref(_id);
    }

    long long actual_layer_flag = 1;

    char _depth   = 0;
    char _enabled = true;

private:
    // Detach the current entity from its parent's children list
    void detach_from_parent();

    template<class T>
    void check_template_argument()
    {
        /*static_assert(std::is_default_constructible<T>(),
						  "Template argument must be default constructible.");*/

        /*static_assert(std::is_base_of<T, Component>(),
										"Template argument must inherit from Components.");*/
    }

    // Variables

    Scene* scene_ = nullptr;
    // We need the reference to the scene to actually delete the entity and its attached components
    // Although to be fair I could act as if there was only 1 scene and directly fetch it
    RefEntity _parent;
    EntityId  _id;

    bool _is_moved = false;

    long long current_layer_ = 0;

    //std::vector<std::string> tags;
    std::vector<corgi::RefEntity> children_;
    std::string                   name_;

    void copy(const Entity& e);
    void move(Entity&& e) noexcept;
};
}    // namespace corgi
