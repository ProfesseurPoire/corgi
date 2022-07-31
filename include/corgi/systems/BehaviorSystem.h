#pragma once

#include <corgi/ecs/System.h>
#include <corgi/ecs/ComponentPool.h>
#include <corgi/components/Behavior.h>

namespace corgi
{
	class BehaviorSystem : public AbstractSystem
	{
		public:


		void update(float ) override
		{
			
		}

	private:

		std::vector<AbstractComponentPool*> pools_;
	};
}