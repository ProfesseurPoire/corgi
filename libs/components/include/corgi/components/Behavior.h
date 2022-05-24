
#pragma once

#include <corgi/ecs/Component.h>
#include <corgi/components/Transform.h>

#include "corgi/ecs/ComponentPool.h"

namespace corgi
{
	class Transform;
	
	class Behavior : public Component
	{
		friend class Scene;
		
	public:
		
		virtual ~Behavior() override{}
		virtual void update();
	};
}
