#include "Inputs.h"
#include "Keyboard.h"
#include "Mouse.h"

namespace corgi
{
	const Gamepad& Inputs::gamepad(int id)const
	{
		return gamepads_[id];
	}

	const Mouse& Inputs::mouse() const noexcept
	{
		return mouse_;
	}

	const Keyboard& Inputs::keyboard() const noexcept
	{
		return keyboard_;
	}

	bool Inputs::is_gamepad_connected(const int index)const noexcept
	{
		return (index < gamepads_.size()&& index >=0u);
	}

	Mouse& Inputs::non_const_mouse()
	{
		return mouse_;
	}

	Keyboard& Inputs::non_const_keyboard()
	{
		return keyboard_;
	}

	void Inputs::update()
	{
		keyboard_.update();
		mouse_.update();

		for(auto& gamepad :  gamepads_)
			gamepad.update();
	}
}
