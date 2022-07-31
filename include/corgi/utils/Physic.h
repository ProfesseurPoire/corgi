#pragma once

#include <corgi/math/Vec2.h>
#include <corgi/math/Vec3.h>

#include <corgi/math/Ray.h>

#include <corgi/utils/types.h>

#include "corgi/ecs/ComponentPool.h"
#include <corgi/components/BoxCollider2D.h>
#include <corgi/ecs/RefEntity.h>

namespace corgi
{
	class ColliderComponent;

	class RaycastResult
	{
		friend class Physic;

	public:

		RaycastResult() = default;

		operator bool()const;

		bool collision_occured = false;
		Vec3 intersection_point;
		Vec3 intersection_normal;
		Ray	 ray;
		ColliderComponent* collider = nullptr;
		Entity* entity{ nullptr };
	};

	class Raycast2DResult
	{
		friend class Physic;

	public:

		Raycast2DResult() = default;

		[[nodiscard]] operator bool()const noexcept
		{
			return collision_occured;
		}

		bool collision_occured{ false };
		Vec2 intersection_point;
		Vec2 intersection_normal;
		Ray2D ray;
		Ref<BoxCollider2D> collider;
		RefEntity entity;
	};

	class Physic
	{
	public:

		// Lifecycle

		// TODO : Not a big fan of having this reference the scene but I need the entities and more
		Physic(Scene& scene);

		// Functions

		Scene& _scene;

		//POD should be initialized at zero
		int_64 layers[64]
		{
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		};

		void set_collision(int layer1, int layer2);
		bool layer_colliding(int layer1, int layer2);

		ComponentPool<Transform>* transforms;

		[[nodiscard]] const RaycastResult raycast(const Vec3& start, const Vec3& direction, float distance, int_64 layer_flag);
		[[nodiscard]] const RaycastResult raycast(const Ray& ray, int_64 layer_flag);

		[[nodiscard]] const Raycast2DResult raycast2D(const Vec2& start, const Vec2& direction, float distance, int_64 layer_flag);
		[[nodiscard]] const Raycast2DResult raycast2D(const Ray2D& ray, int_64 layer_flags);
	};
}
