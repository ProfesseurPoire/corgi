#include "EntityPool.h"

#include <corgi/ecs/Entity.h>
#include <corgi/main/Game.h>

namespace corgi
{
	EntityPool::EntityPool(RefEntity parent) : parent_(parent){}

	EntityPool::~EntityPool()
	{
		//delete parent_;
	}

	RefEntity EntityPool::pop()
	{
		if(stack_.empty())
		{
			return parent_->emplace_back("EntityFromPool");
		}
		else
		{
			auto e = stack_.top();
			stack_.pop();
			return e;
		}
	}

	void EntityPool::push(RefEntity e)
	{
		stack_.push(e);
	}
}
