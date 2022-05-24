#include "CollisionSystem.h"

#include <corgi/ecs/Scene.h>
#include <corgi/ecs/Entity.h>

#include <corgi/utils/Physic.h>
#include <corgi/math/Collisions.h>

#include <corgi/components/ColliderComponent.h>
#include <corgi/components/BoxCollider.h>
#include <corgi/components/BoxCollider2D.h>
#include <corgi/components/Transform.h>

#include <corgi/logger/log.h>

namespace corgi
{
	// Checks if the container collisions has a collision that feature
	// the two collider provided as argument
	static bool contains(const std::vector<Collision>& collisions, const EntityId first_collider, const EntityId second_collider)
	{
		for(auto& collision : collisions)
		{
			if(collision.entity_a == first_collider && collision.entity_b		== second_collider)
			{
				return true;
			}
		}
		return false;
	}

	const std::vector<Collision>& CollisionSystem::collisions()const
	{
		return _collisions;
	}

	const std::vector<Collision>& CollisionSystem::enter_collisions()const
	{
		return _enter_collisions;
	}

	const std::vector<Collision>& CollisionSystem::exit_collisions()const
	{
		return _exit_collisions;
	}

	CollisionSystem::CollisionSystem(Scene& scene, Physic& physic, ComponentPool<BoxCollider>& collider_pool, ComponentPool<BoxCollider2D>& collider_2D_pool,
		ComponentPool<Transform>& transforms)
		: collider_pool_(collider_pool), _collider2D_pool(collider_2D_pool), _scene(scene), _physic(physic), _transforms(transforms)
	{
	}

	// TODO : could also just send the layers
	// and the scene
	void CollisionSystem::update(float elapsed_time)
	{
		_exit_collisions.clear();
		_enter_collisions.clear();

		std::vector<Collision> newCollisions;

		for(auto& collider : collider_pool_)
			collider.colliding = false;

		for (auto& collider : _collider2D_pool)
			collider.colliding = false;

		auto* collider2D_pool = _collider2D_pool.data();
		
		// TODO : There's way too much things happening here. It'd be nice to check a bit what's actually happening
		for(int i=0; i< _collider2D_pool.size(); ++i)
		{
			for(auto j = i+1; j< _collider2D_pool.size(); j++)
			{
				EntityId id_a = _collider2D_pool.entity_id(i);
				EntityId id_b = _collider2D_pool.entity_id(j);

				BoxCollider2D& first_collider	= collider2D_pool[i];
				BoxCollider2D& second_collider	= collider2D_pool[j];

				auto& first_entity	= _scene._entities_contiguous.at(_collider2D_pool.component_index_to_entity_id().at(i));
				auto& second_entity = _scene._entities_contiguous.at(_collider2D_pool.component_index_to_entity_id().at(j));

				if(!first_collider.is_enabled() || !second_collider.is_enabled()|| !first_entity.is_enabled()|| !second_entity.is_enabled())
					continue;

				if (!_physic.layer_colliding(first_collider.layer(), second_collider.layer()))
					continue;
				
				/*
				for (auto& axe : first_collider.axes())
					axes.emplace_back(axe);

				for (auto& axe : second_collider.axes())
					axes.emplace_back(axe);*/

				const auto world_matrix_a = _transforms.get(first_entity.id()).world_matrix();
				const auto world_matrix_b = _transforms.get(second_entity.id()).world_matrix();

				auto world_matrix_a_without_translation = _transforms.get(first_entity.id()).world_matrix();
				auto world_matrix_b_without_translation = _transforms.get(second_entity.id()).world_matrix();

				world_matrix_a_without_translation[12] = 0.0f;
				world_matrix_b_without_translation[12] = 0.0f;

				world_matrix_a_without_translation[13] = 0.0f;
				world_matrix_b_without_translation[13] = 0.0f;

				world_matrix_a_without_translation[14] = 0.0f;
				world_matrix_b_without_translation[14] = 0.0f;

				// For the edges we need to remove the translation component
				// from the world matrix as it fucks up the vectors
				
				auto result = corgi::math::intersect_2D(
					first_collider.positions().data(),
					first_collider.positions().size(),
					second_collider.positions().data(),
					second_collider.positions().size(),
					world_matrix_a, 
					world_matrix_b, 
					first_collider.axes().data(),
					first_collider.axes().size(),
					second_collider.axes().data(),
					second_collider.axes().size(),
					world_matrix_a_without_translation, 
					world_matrix_b_without_translation);

				if(result)
				{
					first_collider.colliding = true;
					second_collider.colliding = true;
					// If they are, we check if the collision has already
					// been registered previously
					
					if (!contains(_collisions, id_a, id_b))
					{
						// If not, we add the collision to our collision list
						// we run the on_enter_ callbacks 
						_enter_collisions.push_back({ id_a,id_b });

						first_collider.on_enter.invoke(first_entity, second_entity, first_collider, second_collider);
						second_collider.on_enter.invoke(second_entity, first_entity, second_collider, first_collider);
					}

					// We register all the collision that occurred during this frame
					newCollisions.push_back({ id_a,id_b, true });

					first_collider.on_collision.invoke(first_entity, second_entity, first_collider, second_collider);
					second_collider.on_collision.invoke(second_entity, first_entity, second_collider, first_collider);
				}
			}
		}

		// Now we compare the collisions that were registered this frame
		// with the collisions that were registered during the previous frame
		for (auto& collision : _collisions)
		{
			if (!_collider2D_pool.contains(collision.entity_b) || !_collider2D_pool.contains(collision.entity_a))
			{
				auto entity_a_name = _scene.get_entity(collision.entity_a).name();
				auto entity_b_name = _scene.get_entity(collision.entity_b).name();

				/*log_warning("One collision exit event was skipped because it couldn't find the colliders " + entity_a_name + " " + entity_b_name);

				if (!_collider2D_pool.contains(collision.entity_a))
					log_warning("Could not find " + entity_a_name);

				if (!_collider2D_pool.contains(collision.entity_b))
					log_warning("Could not find " + entity_a_name);*/

				continue;
			}
			
			if(collision.is_2D)
			{
				// If the collision doesn't exist anymore, then we trigger the exit collision callbacks

				// We check if the current collision still exists in the new collision list.
				// If it doesn't, it means the collision isn't happening anymore, thus we trigger the on_exit event 
				if (!contains(newCollisions, collision.entity_a, collision.entity_b))
				{
					_exit_collisions.push_back(collision);

					auto& ea = _scene.get_entity(collision.entity_a);
					auto& eb = _scene.get_entity(collision.entity_b);

					auto& ca = _collider2D_pool.get(collision.entity_a);
					auto& cb = _collider2D_pool.get(collision.entity_b);
					
					_collider2D_pool.get(collision.entity_a).on_exit.invoke(ea, eb, ca, cb);
					_collider2D_pool.get(collision.entity_a).on_exit.invoke(eb, ea, cb, ca);
				}
			}
		}
		_collisions = newCollisions;
	}
}
