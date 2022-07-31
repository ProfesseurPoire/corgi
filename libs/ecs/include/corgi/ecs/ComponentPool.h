#pragma once

#include <corgi/ecs/EntityId.h>
#include <corgi/ecs/RefEntity.h>

#include <vector>

namespace corgi
{
class Component;

/*!
 * @brief	Stores components in a contiguous array and also map them with an EntityId
 *
 *			Basically, a ComponentPool is just a std::vector with 2 std::map
 *			that associates a component with an EntityId
 *
 *			We want the array to be contiguous for memory considerations
 */
class AbstractComponentPool
{
public:
    friend class ComponentPools;

    // Lifecycle

    AbstractComponentPool()          = default;
    virtual ~AbstractComponentPool() = default;

    // Modifiers

    /*!
	 * @brief	Adds a component and attach it to an Entity
	 */
    virtual void* add(EntityId id, void* comp) = 0;
    virtual void  remove(EntityId id)          = 0;

    // Lookup

    /*!
	 * @brief	Checks if the map contains a component attached to the given entity id
	 *
	 * @param	id	EntityId/Key value of the component to search for
	 */
    [[nodiscard]] virtual bool contains(EntityId id) const noexcept = 0;

    /*!
	 * @brief	Returns the entity id attached at the given location in the container
	 */
    [[nodiscard]] virtual EntityId entity_id(size_t component_index) const = 0;

    [[nodiscard]] virtual size_t entity_id_int(size_t component_index) const = 0;

    // Capacity

    /*!
	 * @brief	Returns the number of components stored by the map
	 */
    [[nodiscard]] virtual size_t size() const noexcept = 0;

    /*!
	 * @brief	Returns true if no components are stored by the map
	 */
    [[nodiscard]] virtual bool empty() const noexcept = 0;

    // Element Access

    /*!
	 * @brief	Returns a pointer to the component attached to the given EntityId
	 *
	 *			Throws an exception if no component attached to the given EntityId was
	 *			found
	 *
	 * @param	id	EntityId of the component we're looking for 
	 */
    virtual void* at(EntityId id) = 0;

    /*!
	 * @brief	Returns a pointer to the component at the given location
	 */
    virtual void*       at(size_t index)       = 0;
    virtual const void* at(size_t index) const = 0;
};

template<class T>
class Ref;

template<class T>
class ConstRef;

template<class T>
class ComponentPool : public AbstractComponentPool
{
public:
    friend class Entity;

    // Lifecycle

    ComponentPool(size_t size = 0) { components_.reserve(size); }

    // Capacity

    [[nodiscard]] size_t size() const noexcept override { return components_.size(); }

    [[nodiscard]] bool empty() const noexcept override { return components_.empty(); }

    T* data() { return components_.data(); }

    // Lookup

    /*!
	 * @brief	Returns the entity id tied to the n-th item in the component map
	 */
    [[nodiscard]] EntityId entity_id(const size_t location) const override
    {
        return EntityId(component_index_to_entity_id_[location]);
    }

    [[nodiscard]] size_t entity_id_int(size_t component_index) const override
    {
        return component_index_to_entity_id_[component_index];
    }

    /*!
	 * @brief	Checks whether or not the component pool store a component tied to the given entity id
	 *
	 *			Returns true if the EntityId given as a parameter contains a component  from
	 *			the current component map. Returns false otherwise
	 */
    [[nodiscard]] bool contains(const EntityId id) const noexcept override
    {
        if(id.id_ >= _entity_id_to_components_vector.size())
            return false;

        return _entity_id_to_components_vector[id.id_] != -1;
    }

    // Modifiers

    /*!
	 * @brief	This will copy the given component and assign it to another
	 *			entity_id. Mainly used for when we need to clone components from an entity
	 */
    void* add(EntityId id, void* comp) override
    {
        if(id.id_ >= _entity_id_to_components_vector.size())
        {
            const auto size = _entity_id_to_components_vector.size();
            _entity_id_to_components_vector.resize(id.id_ + 1u);

            for(auto i = size; i < _entity_id_to_components_vector.size(); i++)
            {
                _entity_id_to_components_vector[i] = EntityId::npos;
            }
        }

        _entity_id_to_components_vector[id.id_] = components_.size();

        component_index_to_entity_id_.emplace_back(id.id_);
        components_.emplace_back(*reinterpret_cast<const T*>(comp));
        return reinterpret_cast<void*>(&components_.back());
    }

    template<class... Args>
    Ref<T> add_param(EntityId id, Args&&... args)
    {
        if(id.id_ >= _entity_id_to_components_vector.size())
        {
            auto size = _entity_id_to_components_vector.size();
            _entity_id_to_components_vector.resize(id.id_ + 1u);

            for(auto i = size; i < _entity_id_to_components_vector.size(); i++)
            {
                _entity_id_to_components_vector[i] = EntityId::npos;
            }
        }

        _entity_id_to_components_vector[id.id_] = components_.size();

        component_index_to_entity_id_.emplace_back(id.id_);
        components_.emplace_back(std::forward<Args>(args)...);
        return Ref<T>(*this, id);
    }

    /*!
	 * @brief	Removes and deletes the component associated with the given @a entity_id
	 */
    void remove(EntityId id) override
    {
        if(!contains(id))
            return;

        const auto index = _entity_id_to_components_vector[id.id_];

        components_.erase(components_.begin() + index);

        component_index_to_entity_id_.erase(component_index_to_entity_id_.begin() +
                                            index);

        _entity_id_to_components_vector[id.id_] = EntityId::npos;

        for(auto& ind : _entity_id_to_components_vector)
        {
            if(ind > index)
                ind--;
        }
    }

    [[nodiscard]] const void* at(size_t index) const override
    {
        return reinterpret_cast<const void*>(&components_[index]);
    }

    void* at(size_t index) override
    {
        return reinterpret_cast<void*>(&components_[index]);
    }

    void* at(EntityId id) override
    {
        return reinterpret_cast<void*>(
            &components_.at(_entity_id_to_components_vector.at(id.id_)));
    }

    [[nodiscard]] T& get(const EntityId id)
    {
        return components_.at(_entity_id_to_components_vector.at(id.id_));
    }

    [[nodiscard]] T& get(size_t index) { return components_.at(index); }

    [[nodiscard]] const T& get(size_t index) const { return components_.at(index); }

    [[nodiscard]] const T& get(const EntityId id) const
    {
        return components_.at(_entity_id_to_components_vector.at(id.id_));
    }

    [[nodiscard]] Ref<T> get_ref(EntityId id) { return Ref<T>(*this, id); }

    [[nodiscard]] ConstRef<T> get_const_ref(EntityId id) const
    {
        return ConstRef<T>(*this, id);
    }

    auto begin() { return components_.begin(); }

    auto end() { return components_.end(); }

    [[nodiscard]] auto begin() const { return components_.cbegin(); }

    [[nodiscard]] auto end() const { return components_.cend(); }

    [[nodiscard]] const std::vector<T>& components() const noexcept
    {
        return components_;
    }

    [[nodiscard]] std::vector<T>& components() noexcept { return components_; }

    [[nodiscard]] std::vector<size_t>& component_index_to_entity_id()
    {
        return component_index_to_entity_id_;
    }

    std::vector<size_t> _entity_id_to_components_vector;

private:
    std::vector<size_t> component_index_to_entity_id_;
    std::vector<T>      components_;
};

/*!
* @brief Keeps a const reference to a component object
*/
template<class T>
class ConstRef
{
public:
    // Lifecycle

    ConstRef() = default;

    // Functions

    [[nodiscard]] operator bool() const noexcept { return entity_id_.operator bool(); }
    [[nodiscard]] const T* operator->() const { return &pool_->get(entity_id_); }

    /*!
	 * @brief	Returns a const reference to the component
	 *
	 *			Actually fetch the reference by interrogating the component_map
	 */
    [[nodiscard]] const T& get() const { return pool_->get(entity_id_); }

private:
    friend class ComponentPool<T>;

    ConstRef(const ComponentPool<T>& pool, EntityId entity_id)
        : pool_(&pool)
        , entity_id_(entity_id)
    {
    }
    const ComponentPool<T>* pool_ {nullptr};
    EntityId                entity_id_;
};

/*!
 * @brief	Used to keep actual references to components.
 *			Since components are stored inside a contiguous array, references
 *			can be lost after insertion or suppression operations.
 *			This will store the entity's id the component is attached to for easy retrieval 
 */
template<class T>
class Ref
{
public:
    // Lifecycle

    Ref() = default;

    // Observers

    /*!
	 * @brief Returns false if the object doesn't reference anything
	 */
    [[nodiscard]] operator bool() const noexcept { return entity_id_.operator bool(); }

    [[nodiscard]] T*       operator->() { return &pool_->get(entity_id_); }
    [[nodiscard]] const T* operator->() const { return &pool_->get(entity_id_); }

    [[nodiscard]] T&       get() { return pool_->get(entity_id_); }
    [[nodiscard]] const T& get() const { return pool_->get(entity_id_); }

    [[nodiscard]] T&       operator*() { return pool_->get(entity_id_); }
    [[nodiscard]] const T& operator*() const { return pool_->get(entity_id_); }

    //private:

    friend class ComponentPool<T>;

    Ref(ComponentPool<T>& pool, EntityId entity_id)
        : pool_(&pool)
        , entity_id_(entity_id)
    {
    }
    ComponentPool<T>* pool_ {nullptr};
    EntityId          entity_id_;
};
}    // namespace corgi
