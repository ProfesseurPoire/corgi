#include <corgi/inputs/Mouse.h>

using namespace corgi;

bool Mouse::has_moved() const
{
    return move_event_;
}

int Mouse::wheelDelta() const noexcept
{
    return mWheelDelta;
}

void Mouse::update()
{
    move_event_ = false;
    mWheelDelta = 0;

    for(int i = 0; i < 3; i++)
    {
        buttons_down_[i] = false;
        buttons_up_[i]   = false;
    }
}

Mouse::MousePoint Mouse::delta() const noexcept
{
    return MousePoint {x_ - previous_x_, y_ - previous_y_};
}

int Mouse::previous_x() const
{
    return previous_x_;
}

int Mouse::previous_y() const
{
    return previous_y_;
}

Mouse::Mouse()
{
    for(int i = 0; i < 3; i++)
    {
        buttons_[i]      = false;
        buttons_down_[i] = false;
        buttons_up_[i]   = false;
    }
}

int Mouse::x() const
{
    return x_;
}

int Mouse::y() const
{
    return y_;
}

Mouse::MousePoint Mouse::position() const noexcept
{
    return Mouse::MousePoint {x_, y_};
}

Mouse::MousePoint Mouse::previous_position() const noexcept
{
    return Mouse::MousePoint {previous_x_, previous_y_};
}

bool Mouse::is_button_down(const Button button) const
{
    return buttons_down_[static_cast<size_t>(button)];
}

bool Mouse::is_button_pressed(Button button) const
{
    return buttons_[static_cast<size_t>(button)];
}

bool Mouse::is_button_up(Button button) const
{
    return buttons_up_[static_cast<size_t>(button)];
}
