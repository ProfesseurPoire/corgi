#pragma once

#include <corgi/inputs/Gamepad.h>
#include <corgi/inputs/Keyboard.h>
#include <corgi/inputs/Mouse.h>

namespace corgi
{
class Window;

/**
	 * @brief	Stores the input's states and events to be reused later on
	 * 
	 * 			Basically, at the begining of each frame, the game will poll the events
	 * 			to update the inputs
	 */
class Inputs
{
public:
    //TODO : Probably should be defined when building the game
    static constexpr int MAX_GAMEPAD_COUNT = 4;

    // Friends

    friend class Window;
    friend class Game;

    // Lifecycle

    Inputs() = default;

    Inputs(const Inputs& other) = delete;
    Inputs(Inputs&& other)      = delete;

    Inputs& operator=(const Inputs& other) = delete;
    Inputs& operator=(Inputs&& other)      = delete;

    // Functions

    /*!
		 * @brief 	Returns a reference to the gamepad identified by @a id
		 */
    [[nodiscard]] const Gamepad& gamepad(int id) const;

    /*!
		 * @brief 	Returns a const reference to the mouse current state
		 */
    [[nodiscard]] const Mouse& mouse() const noexcept;

    /*!
		 * @brief 	Returns a const reference to the keyboard current state
		 */
    [[nodiscard]] const Keyboard& keyboard() const noexcept;

    /*!
		 * @brief	Returns true if a gamepad exist at the given ID
		 */
    [[nodiscard]] bool is_gamepad_connected(int index) const noexcept;

protected:
    void init_gamepads();

    Keyboard keyboard_;
    Mouse    mouse_;

    Gamepad gamepads_[MAX_GAMEPAD_COUNT];

    /**
		 * @brief 	Poll events and update the inputs states
		 * 
		 * 			This function should only be called once per frame by the game loop
		 * 			This is why we keep it protected to avoid any misuse.
		 * 
		 * 			The only alternative I see would be move this function into Game.h
		 * 			as "update_inputs" and making the Inputs class only a struct
		 */
    void update();

    Mouse&    non_const_mouse();
    Keyboard& non_const_keyboard();
};
}    // namespace corgi
