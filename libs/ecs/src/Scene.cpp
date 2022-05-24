#include <corgi/ecs/Scene.h>
#include <corgi/ecs/Entity.h>


namespace corgi
{

Scene::Scene()
{
	static const std::string root = "root";
	root_ = new_entity(*this, root);
}

RefEntity Scene::clone(RefEntity base, RefEntity par)
{
	RefEntity cloned_entity;

	// If the par argument isn't null, we manually set it as
	// a parent for the cloned entity

	if (par)
	{
		cloned_entity = new_entity(par, base->name());
	}
	else // Otherwise, the copy will be a sibling of the copied
	{
		if (base->_parent)
			cloned_entity = new_entity(base->_parent, base->name() );
		else
			cloned_entity = new_entity(*base->scene_, base->name() );
	}

	// Copy the components

	for (auto& pair : base->scene_->component_maps())
	{
		if (pair.second->contains(base->_id))
		{
			pair.second->add(cloned_entity->_id, pair.second->at(base->_id));
		}
			
	}

	// Copy the children
	for (const auto& child : base->children_)
		clone(child, cloned_entity);

	return cloned_entity;
}

RefEntity Scene::new_entity(Scene& scene, const std::string& name)
{
	auto id = get_next_id();
	_entities_contiguous[id.id_] = Entity(id, scene, name.c_str());
	return RefEntity(*this, _entities_contiguous[id.id_]);
}

RefEntity Scene::new_entity(RefEntity parent, const std::string& name)
{
	auto id = get_next_id();
	auto ref = RefEntity(*this, _entities_contiguous[id.id_] = Entity(id, parent, name.c_str()));
	parent->children_.emplace_back(ref);
	return ref;
}

void Scene::remove_entity(RefEntity entity)
{
	std::cout << "Remove entity" << entity->name() << std::endl;
	// When we delete an entity, we simply put back its id in the
	// queue and remove its components
	_usable_ids.push_front(entity->id());
	entity->scene_->unregister_entity_from_component_pools(entity->id());

	for (auto child : entity->children())
		remove_entity(child);
}

EntityId Scene::get_next_id()
{
	if (_usable_ids.empty())
		grow_ids();

	auto id = _usable_ids.front();
	_usable_ids.pop_front();

	return id;
}

void Scene::grow_ids()
{
	const auto start_new_ids = _existing_id_count;
	const auto end_new_ids = start_new_ids + _existing_id_count + 1 * 2;

	for (auto i = start_new_ids; i < end_new_ids; i++)
		_usable_ids.push_back(EntityId(i));

	_existing_id_count += _existing_id_count + 1 * 2;

	_entities_contiguous.resize(end_new_ids);
}

Scene::~Scene()
{
	remove_entity(root_);
}

RefEntity Scene::find(std::string_view name)
{
	return root_->find(name.data());
}

std::map<std::type_index, std::unique_ptr<AbstractSystem>>& Scene::systems()
{
	return systems_;
}

void Scene::unregister_entity_from_component_pools(EntityId id)
{
	for(auto& [key, pool] : _component_maps)
		pool->remove(id);
}

void Scene::before_update(float elapsed_time)
{
	_elapsed_time = elapsed_time;

	for(auto key : systems_order_)
		systems_[key]->before_update(elapsed_time);
}

void Scene::update(const float elapsed_time)
{
	_elapsed_time = elapsed_time;

	for(auto key : systems_order_)
		systems_[key]->update(elapsed_time);
}

void Scene::after_update(const float elapsed_time)
{
	_elapsed_time = elapsed_time;

	for(auto key : systems_order_)
		systems_[key]->after_update(elapsed_time);
}

/*Canvas& Scene::new_canvas()
{
	return *canvas_.emplace_back(std::make_unique<Canvas>()).get();
}

void Scene::add_canvas(Canvas&& canvas)
{
	canvas_.push_back(std::make_unique<Canvas>(canvas));
}

void Scene::add_canvas(const Canvas& canvas)
{
	canvas_.push_back(std::make_unique<Canvas>(canvas));
}*/

RefEntity Scene::new_entity(const std::string& name, EntityCreationFlag flags)
{
	return new_entity(root_, name);
}

RefEntity Scene::append(const std::string& name)
{
	return new_entity(name);
}

Entity& Scene::get_entity(EntityId id)
{
	return _entities_contiguous.at(id.id_);
}

void Scene::clear()
{
	root_->clear();
}
}
