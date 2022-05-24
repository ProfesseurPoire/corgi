#include <corgi/ecs/ComponentPools.h>

namespace corgi
{
	void ComponentPools::remove(const std::type_info& component_type)
	{
		pools_.erase(component_type);
	}

	int ComponentPools::size() const noexcept
	{
		return pools_.size();
	}

	const AbstractComponentPool* ComponentPools::get(std::type_index t) const
	{
		return pools_.at(t).get();
	}

	AbstractComponentPool* ComponentPools::get(std::type_index t)
	{
		return pools_.at(t).get();
	}

	std::map<std::type_index, std::unique_ptr<AbstractComponentPool>>::iterator ComponentPools::end()
	{
		return pools_.end();
	}

	bool ComponentPools::contains(std::type_index t)const noexcept
	{
		return pools_.contains(t);
	}

	std::map<std::type_index, std::unique_ptr<AbstractComponentPool>>::iterator ComponentPools::begin()
	{
		return pools_.begin();
	}
}
