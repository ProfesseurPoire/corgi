#pragma once

#include <stack>
#include <corgi/ecs/RefEntity.h>

namespace corgi
{
	/*!
	 * @brief	The goal of the entity pool is to avoid unnecessary memory
	 *			allocation by creating a fixed number of Entity and reusing them once. This
	 *			is especially useful for things like particles or projectiles.
	 */
	class EntityPool
	{
	public:

	// Lifecycle

		/*!
		 * @brief	Entities created by the entity pool will inherit from the given parameter,
		 *			and the entity pool will create x stuff
		 */
		EntityPool(RefEntity parent);
		~EntityPool();

	//  Functions
		
		/*!
		 * @brief	Returns an entity from the pool. If no entity is available
		 *			a new one is created
		 */
		RefEntity pop();
		void push(RefEntity e);

	private:

		std::stack<RefEntity> stack_;
		RefEntity parent_;
	};
}
