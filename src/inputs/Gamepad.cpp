#include <SDL2/SDL.h>
#include <corgi/inputs/Gamepad.h>

using namespace corgi;

void corgi::Gamepad::init(int id)
{
    SDL_JoystickEventState(SDL_ENABLE);
    joystick_impl_ = SDL_JoystickOpen(id_);

    const int num_button =
        SDL_JoystickNumButtons(static_cast<SDL_Joystick*>(joystick_impl_));
    const int num_axes = SDL_JoystickNumAxes(static_cast<SDL_Joystick*>(joystick_impl_));

    axes_.resize(num_axes);
    buttons_.resize(num_button);
    enabled_ = true;
}

void corgi::Gamepad::finalize()
{
    enabled_ = false;
    SDL_JoystickClose(static_cast<SDL_Joystick*>(joystick_impl_));
}

corgi::Gamepad::~Gamepad()
{
    if(enabled_)
    {
        SDL_JoystickClose(static_cast<SDL_Joystick*>(joystick_impl_));
    }
}

void corgi::Gamepad::update()
{
    for(int i = 0; i < buttons_.size(); ++i)
    {
        buttons_[i] =
            static_cast<Gamepad::Status>(static_cast<char>(buttons_[i]) & 0b00000100);
    }
}

bool corgi::Gamepad::button(int id) const
{
    return (static_cast<char>(buttons_[id]) &
            static_cast<char>(corgi::Gamepad::Status::Pressed)) != 0;
}

bool corgi::Gamepad::button_down(int id) const
{
    return (static_cast<char>(buttons_[id]) &
            static_cast<char>(corgi::Gamepad::Status::Down)) != 0;
}

bool corgi::Gamepad::button_up(const int id) const
{
    return (static_cast<char>(buttons_[id]) &
            static_cast<char>(corgi::Gamepad::Status::Up)) != 0;
}

size_t corgi::Gamepad::button_count() const
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