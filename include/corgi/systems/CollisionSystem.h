#pragma once

#include <corgi/ecs/System.h>
#include <corgi/ecs/ComponentPool.h>

#include <vector>

namespace corgi
{
	class ColliderComponent;
	class Physic;

	class BoxCollider;
	class BoxCollider2D;
	class Scene;
	class Transform;

	struct Collision
	{
		EntityId entity_a;
		EntityId entity_b;
		bool is_2D{ true };
	};

	// Check the collisions for every collider in the game, and fire their callbacks
	// functions
	class CollisionSystem : public AbstractSystem
	{
	public:

		// TODO : Would be better to directly use GameScene here 
		CollisionSystem(Scene& scene, Physic& physic, ComponentPool<BoxCollider>& collider_pool, ComponentPool<BoxCollider2D>& collider2D_pool,
			ComponentPool<Transform>& transforms);

		// Scene to get back the colliders, Physic to get back
		// the collision layers
		void update(float elapsed_time) override;

		[[nodiscard]] const std::vector<Collision>& collisions()const;
		[[nodiscard]] const std::vector<Collision>& enter_collisions()const;
		[[nodiscard]] const std::vector<Collision>& exit_collisions()const;

	private:

		std::vector<Collision> _collisions;
		std::vector<Collision> _enter_collisions;
		std::vector<Collision> _exit_collisions;

		ComponentPool<BoxCollider>&		collider_pool_;
		ComponentPool<BoxCollider2D>&	_collider2D_pool;
		ComponentPool<Transform>&		_transforms;
		Scene& _scene;
		Physic& _physic;
	};
}