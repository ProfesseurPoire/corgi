#include <corgi/components/ColliderComponent.h>

#include <corgi/math/Collisions.h>
#include <corgi/ecs/Entity.h>
#include <corgi/components/Transform.h>
#include <corgi/resources/Mesh.h>

#include <assert.h>

namespace corgi
{
	ColliderComponent::ColliderComponent(int layer) : layer_(layer) {}

	void ColliderComponent::layer(int layer_id)
	{
		// Can't have a layer > 64, since I use a 64 bit variable
		// as my collision layer mask
		assert(layer_id < 64 && layer_id >= 0);
		layer_ =  layer_id;
	}

	int ColliderComponent::layer() const
	{
		return layer_;
	}

	bool ColliderComponent::collide(const Entity& a, const Entity& b, ColliderComponent* col)
	{
		return math::intersect_3D
		(
			a.get_component<Transform>()->world_matrix(),
			_mesh->indexes().data(),
			_mesh->indexes().size(),
			_mesh->vertices().data(),
			3,
			0,
			_mesh->vertex_count(),
			b.get_component<Transform>()->world_matrix(),
			col->_mesh->vertices().data(),
			col->_mesh->indexes().data(),
			col->_mesh->indexes().size(),
			3,
			0,
			col->_mesh->vertex_count()
		);
	}
}