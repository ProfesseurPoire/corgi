#pragma once

#include <corgi/ecs/System.h>
#include <corgi/ecs/ComponentPool.h>

namespace corgi
{
	class Transform;
	class Scene;
	
	class TransformSystem : public AbstractSystem
	{
	public:

		TransformSystem(Scene& scene, ComponentPool<Transform>& transforms);

		void update_component(Transform& transform, Entity& entity);

	protected:

		void update(float elapsed_time) override;
		
	private:

		int depth(const Transform& transform, const Entity& entity, int val=0);
		
		ComponentPool<Transform>& transforms_;
		Scene& _scene;
	};
}