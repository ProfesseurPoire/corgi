#include <corgi/ecs/Entity.h>

#include <algorithm>
#include <cstring>

#include "corgi/ecs/Component.h"

namespace corgi
{
	Entity::Entity(EntityId id, Scene& scene, const char* name) :
		_id(id), scene_(&scene), name_(name)
	{
		_depth = 0;
	}

	Entity::Entity(const Entity& entity)
	{
		copy(entity);
	}

	Entity::Entity(Entity&& entity)noexcept
	{
		move(std::move(entity));
	}

	Entity::~Entity()
	{
		
	}

	void Entity::Iterator::increment_breadth_first()
	{
		current_node_ = queue_.front();
		queue_.pop_front();

		// TODO : Maybe check if this can be done with simply a
		// insert or something

		for(int i=0; i< current_node_->children().size(); i++)
		{
			queue_.push_back(current_node_->children()[i]);
		}
	}

	bool Entity::Iterator::operator!=(const Iterator& iterator)const noexcept
	{
		return current_node_ != iterator.current_node_;
	}

	bool Entity::Iterator::operator==(const Iterator& iterator)const
	{
		return current_node_ == iterator.current_node_;
	}

	Entity::Iterator& Entity::Iterator::operator++()
	{
		if (queue_.empty())
		{
			current_node_.reset();
			return *this;
		}
		switch (mode_)
		{
		case IteratorMode::BreadthFirst:
			increment_breadth_first();
			break;

		case IteratorMode::DepthFirst:
			increment_depth_first();
			break;
		}
		return *this;
	}

	void Entity::Iterator::increment_depth_first()
	{
		current_node_ = queue_.front();
		queue_.pop_front();

		for(int i = current_node_->children().size()-1; i>=0; i--)
		{
			queue_.push_front(current_node_->children()[i]);
		}
	}

	Entity::Iterator::Iterator(Entity& node, IteratorMode mode) :
		mode_(mode)
	{
		std::ranges::transform
		(
			node.children_,
			std::back_inserter(queue_),
			[](RefEntity n)->RefEntity { return n; }
		);
		operator++();
	}

	// TODO : THis is broken 
	void Entity::copy(const Entity& e)
	{
		_parent			= e._parent;
		//enabled_		= e.enabled_;
		current_layer_	= e.current_layer_;
		name_			= e.name_;
		//transform_._entity = this;

		// It's either that or have a virtual function in Components so :eyes:
		/*for(auto& component : e._component_maps)
		{
			_component_maps.emplace(component.first, component.second->clone(*this));
		}*/

		/*for (auto& child : e.children_)
		{
			children_.push_back(std::unique_ptr<Entity>(new Entity(*child)));
		}*/
	}

	void Entity::move(Entity&& other)noexcept
	{
		_parent			= other._parent;
		children_		= std::move(other.children_);
		name_			= std::move(other.name_);
		current_layer_	= other.current_layer_;
		_id				= other.id();
		scene_			= other.scene_;
		_enabled		= other._enabled;
		_depth			= other._depth;

		other._is_moved = true;
	}

	Entity& Entity::operator=(const Entity& entity)
	{
		copy(entity);
		return *this;
	}

	Entity& Entity::operator=(Entity&& entity) noexcept
	{
		move(std::move(entity));
		return *this;
	}

	EntityId Entity::id()const noexcept
	{
		return _id;
	}

	RefEntity Entity::find(const char* name) noexcept
	{
		const auto val = std::find_if(begin(), end(), [&](RefEntity e)->bool 
		{ 
			return std::strcmp(e->name(), name) == 0;
		}).get();

		if(!val)
			return RefEntity();
		return val;
	}

	/*Behavior* Entity::behavior()
	{
		for (auto& pool : scene_->pools())
		{
			if (pool.second->has_component(id_))
			{
				auto* c = pool.second.get()->at_id(id_);
				auto* b = dynamic_cast<Behavior*>(c);

				if (b)
				{
					return b;
				}
			}
		}
		return nullptr;
	}*/

	void Entity::remove_child(RefEntity e)
	{
		for(int i=0; i< children_.size(); i++)
		{
			if(children_[i]==e)
			{
				children_.remove_at(i);
			}
		}

		/*children_.erase
		(
			std::remove_if
			(
				children_.begin(),
				children_.end(),
				[&](RefEntity p)->bool
				{
					return p == e;
				}
			),
			children_.end()
		);*/
	}

	

	Entity::Entity(EntityId id, RefEntity parent, const char* name)
		: _parent(parent), name_(name), _id(id)
	{
		if(!parent)
			throw std::invalid_argument("Parent argument is null");
		scene_ = parent->scene_;
		_depth = parent->_depth + 1;
	}

	bool Entity::has_component(std::type_index t) const noexcept
	{
		// We first check if there's a pool that could hold the component 
		if (!scene_->component_maps().contains(t))
			return false;
		return scene_->component_maps().get(t)->contains(_id); 
	}

	[[nodiscard]] const char* Entity::name()const
	{
		return name_.c_str();
	}

	long long Entity::current_layer()const
	{
		return current_layer_;
	}

	void Entity::rename(const char* n)
	{
		name_ = n;
	}

	void Entity::current_layer(int cl)
	{
		current_layer_ = cl;
		actual_layer_flag = static_cast<long long>(1) << cl;
	}

	long long Entity::actual_layer()
	{
		return actual_layer_flag;
	}

	void Entity::enable()
	{
		_enabled = true;
		
		for (auto c : *this)
			c->_enabled = true;
	}

	void Entity::disable()
	{
		_enabled = false;

		for (auto c : *this)
			c->_enabled = false;
	}

	bool Entity::is_enabled()const noexcept
	{
		return _enabled;
	}
	
	RefEntity Entity::emplace_back(const char* name)
	{
		return scene_->new_entity(RefEntity(*scene_, *this), name);
	}

	// Detach the current entity from its parent's children list
	void Entity::detach_from_parent()
	{
		//auto it = std::ranges::find_if(parent_->children_,
		//	[&](std::unique_ptr<Entity>& p) { return p.get() == this; });

		//it->release();					// We release the pointer
		//parent_->children_.erase(it);	// We delete the unique_ptr
	}

	RefEntity Entity::parent() noexcept
	{
		return _parent;
	}

	void Entity::remove_component_of_type(std::type_index index)
	{
		if (!scene_->component_maps().contains(index))
		{
			throw std::invalid_argument("Could not remove component of given type : no pool exist with that component");
		}

		if (!has_component(index))
		{
			throw std::invalid_argument("Could not remove component of given type : Entity doesn't have a component of type T");
		}
		scene_->component_maps().get(index)->remove(_id);
	}

	const Vector<RefEntity>& Entity::children()const noexcept
	{
		return children_;
	}

	Vector<RefEntity>& Entity::children() noexcept
	{
		return children_;
	}

	Scene& Entity::scene() noexcept
	{
		return *scene_;
	}

	void Entity::parent(RefEntity newparent_)
	{
		//if (!newparent_)
		//{
		//	if (parent_)
		//	{
		//		detach_from_parent();
		//	}

		//	// TODO : Check this out, I'm hacking a bit here
		//	parent_ = scene().root();
		//	scene().root()->push_back(this);
		//}
		//else
		//{
		//	if (newparent_ != parent_)
		//	{
		//		if (parent_)
		//			detach_from_parent();

		//		newparent_->push_back(this);
		//		parent_ = newparent_;
		//	}
		//}
	}

	void Entity::clear() noexcept
	{
		for(auto child : children_)
		{
            child->clear();
			scene_->remove_entity(child); 
		}
        children_.clear();
	}

	int Entity::depth(int d) const noexcept
	{
		if (_parent)
			return _parent->depth(++d);
		return d;
	}

	void Entity::remove()
	{
		scene_->remove_entity(RefEntity(*scene_, *this));
	}
}
