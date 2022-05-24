#pragma once

#include <corgi/containers/Vector.h>

namespace corgi
{
	class Gamepad
	{
		friend class Window;
		
	public:

		enum class Hat{ LeftUp, Up, RightUp, Left, Centered, Right, LeftDown, Down, RightDown };

	//   Constructors
		
		Gamepad(int id);
		~Gamepad();

	//  Functions

		void update();

		[[nodiscard]] bool button(int id)const;
		[[nodiscard]] bool button_down(int id)const;
		[[nodiscard]] bool button_up(int id)const;

		[[nodiscard]] int button_count()const;
		[[nodiscard]] float axis(int id)const;

		[[nodiscard]] Hat hat()const;

	private:

		Vector<bool> buttons_;
		Vector<bool> buttons_down_;
		Vector<bool> buttons_up_;

		Vector<float> axes_;

		Hat hat_ = Hat::Centered;
		
		int id_;

		void* joystick_impl_ = nullptr;
	};
}
