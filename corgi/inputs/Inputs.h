#pragma once


#include <corgi/inputs/Keyboard.h>
#include <corgi/inputs/Gamepad.h>
#include <corgi/inputs/Mouse.h>

#include <corgi/containers/Vector.h>

namespace corgi
{
    class Window;
	
    class Inputs
    {
    public:

	// Friends

		friend class Window;
		friend class Game;
		
	// Lifecycle

		Inputs()=default;

		Inputs(const Inputs& other)=delete;
		Inputs(Inputs&& other)=delete;

		Inputs& operator=(const Inputs& other)=delete;
		Inputs& operator=(Inputs&& other)=delete;

	// Functions

		/*!
		 * @brief 	Returns a reference to the gamepad identified by @a id
		 */
		[[nodiscard]] const Gamepad& gamepad(int id)const;

		/*!
		 * @brief 	Returns a const reference to the mouse current state
		 */
    	[[nodiscard]] const Mouse& mouse()const noexcept;

		/*!
		 * @brief 	Returns a const reference to the keyboard current state
		 */
    	[[nodiscard]] const Keyboard& keyboard()const noexcept;
    	
		/*!
		 * @brief	Returns true if a gamepad exist at the given ID
		 */
		[[nodiscard]] bool is_gamepad_connected(int index)const noexcept;

    protected:

		Keyboard				keyboard_;
		Mouse					mouse_;
		Vector<Gamepad>			gamepads_;

		void update();

		Mouse& non_const_mouse();
		Keyboard& non_const_keyboard();
    };
}
