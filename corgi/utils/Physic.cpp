#include "Physic.h"

#include <corgi/math/Ray.h>
#include <corgi/math/Collisions.h>

#include <corgi/components/BoxCollider.h>
#include <corgi/components/BoxCollider2D.h>
#include <corgi/components/Transform.h>

#include <corgi/ecs/Entity.h>
#include <corgi/main/Game.h>

#include <limits>

namespace corgi
{
	RaycastResult::operator bool()const
	{
		return collision_occured;
	}

	Physic::Physic(Scene& scene) : _scene(scene)
	{
	}

	void Physic::set_collision(int layer1, int layer2)
	{
		layers[layer1] |= int_64(1) << layer2;
		layers[layer2] |= int_64(1) << layer1;
	}

	bool Physic::layer_colliding(int layer1, int layer2)
	{
		return ((layers[layer1] & (int_64(1) << layer2)) != 0);
	}

	const RaycastResult Physic::raycast(const Ray& ray, int_64 layer_flag)
	{
		return raycast(ray.start, ray.direction, ray.length, layer_flag);
	}

	const Raycast2DResult Physic::raycast2D(const Vec2& start, const Vec2& direction, float distance, int_64 layer_flag)
	{
		Raycast2DResult result;
		Ray2D ray = { direction, start,  distance };

		result.ray = ray;

		// Get the colliders
		Vec3 intersection_point;
		Vec3 intersection_normal;

		float min_length = std::numeric_limits<float>::max();

		auto box2D_collider_map = Game::instance().scene().component_maps().get<BoxCollider2D>();
		
		int i = 0;
		for (auto& collider : *box2D_collider_map)
		{
			auto entity_id	= box2D_collider_map->component_index_to_entity_id().at(i);
			auto& entity	= _scene._entities_contiguous.at(entity_id);

			if (!collider.is_enabled() || !entity.is_enabled())
			{
				i++;
				continue;
			}

			int_64 v = 1;

			if (((v << (collider.layer_)) & layer_flag) != 0)
			{
				auto& transform = transforms->get(entity.id());

				const auto& edges = collider.edges();

				for(size_t i=0; i< edges.size(); i+=2)
				{
					auto a = edges[i];
					auto b = edges[i + 1];

					a = transform.world_matrix() * a;
					b = transform.world_matrix() * b;

					Vec2 intersection_point;
					
					if(Vec2::segment_intersect(a, b, start, start + (direction*distance), intersection_point))
					{
						result.intersection_point = intersection_point;
						result.entity	= RefEntity(_scene, entity);
						result.collider = Game::instance().scene().component_maps().get<BoxCollider2D>()->get_ref(EntityId(entity_id));
						result.collision_occured = true;
						return result;
					}
				}
			}
			i++;
		}
		return result;
	}

	const Raycast2DResult Physic::raycast2D(const Ray2D& ray, int_64 layer_flags)
	{
		return raycast2D(ray.start, ray.direction, ray.length, layer_flags);
	}

	const RaycastResult Physic::raycast(const Vec3& start, const Vec3& direction, float length, int_64 layer)
	{
		RaycastResult result;
		Ray ray(start, direction, length);

		result.ray = ray;

		// Get the colliders
		Vec3 intersection_point;
		Vec3 intersection_normal;

		float min_length = std::numeric_limits<float>::max();

		int i = 0;
		for (auto& collider : *Game::instance().scene().component_maps().get<BoxCollider>())
		{
			auto entity_id	= Game::instance().scene().component_maps().get<BoxCollider>()->component_index_to_entity_id().at(i);
			auto& entity	= _scene._entities_contiguous.at(entity_id);
			
			if(! collider.is_enabled() || !entity.is_enabled() )
			{
				i++;
				continue;
			}
			
			int_64 v = 1;

			if ((( v << (collider.layer_))& layer) != 0)
			{
				auto mesh = collider._mesh;
				auto& transform = transforms->get(entity.id());
				
				if (
					math::intersect_with_collider
					(
						0,
						3, 
						transform.world_matrix(),
						transform.world_matrix().inverse(),
						mesh->indexes().data(),
						mesh->indexes().size(),
						mesh->vertices().data(),
						collider.normals.data(),
						ray,
						intersection_point, intersection_normal
					)
				)
				{
					if ((intersection_point - start).length() < min_length)
					{
						min_length = (intersection_point - start).length();
						result.collision_occured	= true;
						result.intersection_point	= intersection_point;
						result.collider				= &collider;
						result.intersection_normal	= intersection_normal;
						result.entity = &entity;
					}
				}
			}
			i++;
		}
		return result;
	}
}