#include <corgi/ecs/RefEntity.h>
#include <corgi/ecs/Entity.h>
#include <corgi/ecs/Scene.h>

namespace corgi
{
	RefEntity::RefEntity(Scene& scene, const Entity& e)
		: _id(e.id()), _scene(&scene) {}

	RefEntity::RefEntity(Entity& entity):
		_scene(&entity.scene()), _id(entity.id())
	{
		
	}

	RefEntity::RefEntity(RefEntity&& other) noexcept
		: _id(other._id), _scene(other._scene)
	{
		other._scene	= nullptr;
		other._id		= EntityId();
	}

	RefEntity& RefEntity::operator=(const RefEntity& other)
	{
		_scene	= other._scene;
		_id		= other._id;
		return *this;
	}

	RefEntity& RefEntity::operator=(RefEntity&& other) noexcept
	{
		_id		= other._id;
		_scene	= other._scene;
		
		other._id = EntityId();
		return *this;
	}

	RefEntity::RefEntity(const RefEntity& other) :
		_id(other._id),
		_scene(other._scene){}

	RefEntity::operator bool() const noexcept
	{
		return _id.operator bool();
	}

	/*EntityId RefEntity::entity_id() const
	{
		return EntityId(_id);
	}*/

	void RefEntity::reset()
	{
		_id = EntityId();
	}

	RefEntity Entity::Iterator::get()
	{
		return current_node_;
	}

	Entity* RefEntity::operator->()
	{
		return &_scene->get_entity(_id);
	}

	const Entity* RefEntity::operator->() const
	{
		return &_scene->get_entity(_id);
	}

	Entity& RefEntity::operator*()
	{
		return _scene->get_entity(_id);
	}

	const Entity& RefEntity::operator*() const
	{
		return _scene->get_entity(_id);
	}

	bool RefEntity::operator==(const RefEntity& other) const noexcept
	{
		return _id == other._id;
	}
}
