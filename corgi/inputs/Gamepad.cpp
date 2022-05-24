#include "Gamepad.h"

#include <SDL2/SDL.h>

corgi::Gamepad::Gamepad(int id)
    : id_(id)
{
    SDL_JoystickEventState(SDL_ENABLE);
    joystick_impl_ = SDL_JoystickOpen(id_);

	const int num_button    = SDL_JoystickNumButtons(static_cast<SDL_Joystick*>(joystick_impl_));
	const int num_axes      = SDL_JoystickNumAxes(static_cast<SDL_Joystick*>(joystick_impl_));
	
	axes_.resize(num_axes);
    buttons_.resize(num_button);
    buttons_down_.resize(num_button);
    buttons_up_.resize(num_button);
}

corgi::Gamepad::~Gamepad()
{
    SDL_JoystickClose(static_cast<SDL_Joystick*>( joystick_impl_));
}

void corgi::Gamepad::update()
{
    for (int i = 0; i < buttons_.size(); ++i)
    {
        buttons_down_[i]    = false;
        buttons_up_[i]      = false;
        buttons_[i]         = false;
    }
}

bool corgi::Gamepad::button(int id) const
{
    return buttons_[id];
}

bool corgi::Gamepad::button_down(int id) const
{
    return buttons_down_[id];
}

bool corgi::Gamepad::button_up(const int id) const
{
    return buttons_up_[id];
}

int corgi::Gamepad::button_count() const
{
    return buttons_.size();
}

float corgi::Gamepad::axis(int id) const
{
    return axes_[id];
}

corgi::Gamepad::Hat corgi::Gamepad::hat() const
{
    return hat_;
}