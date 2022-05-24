#include <corgi/ecs/Component.h>
#include <corgi/ecs/Entity.h>

namespace corgi
{

	Component::~Component() {}


	bool Component::is_enabled() const
	{
		return is_enabled_;
	}

	void Component::enable()
	{
		is_enabled_=true;
	}

	void Component::disable()
	{
		is_enabled_=false;
	}

	void Component::is_enabled(bool value)
	{
		is_enabled_ = value;
	}

	int Component::register_type_name(std::type_index t, std::string s)
	{
		type_name.emplace(t,s);
		return 0;
	}
}
