#pragma once

#include <corgi/ecs/System.h>
#include <corgi/ecs/ComponentPool.h>

#include <corgi/components/ParticleEmitter.h>

namespace corgi
{
	class ParticleEmitterSystem : public AbstractSystem
	{
	public:

		ParticleEmitterSystem(ComponentPool<ParticleEmitter>& component_pool)
			:components_(component_pool)
		{

		}

		void update(float )override
		{
			for (auto& a : components_)
			{
				if (a.enabled())
				{
					a.update();
				}
			}
		}

	private:

		ComponentPool<ParticleEmitter>& components_;
	};
}