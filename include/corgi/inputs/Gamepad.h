#pragma once

#include <vector>

namespace corgi
{
class Gamepad
{
    friend class Game;

public:
    enum class Hat : unsigned char
    {
        LeftUp,
        Up,
        RightUp,
        Left,
        Centered,
        Right,
        LeftDown,
        Down,
        RightDown
    };

    //   Constructors

    Gamepad() = default;
    ~Gamepad();

    //  Functions

    /**
	 * \brief 	Needs to be called after construction because we need SDL initialized
	 * 			to create a gamepad
	 * 
	 * \param id 
	 */
    void init(int id);
    void finalize();

    void update();

    [[nodiscard]] bool button(int id) const;
    [[nodiscard]] bool button_down(int id) const;
    [[nodiscard]] bool button_up(int id) const;

    [[nodiscard]] size_t button_count() const;

    [[nodiscard]] float axis(int id) const;

    [[nodiscard]] Hat hat() const;

    enum class Status : char
    {
        None           = 0b00000000,
        Up             = 0b00000001,
        Down           = 0b00000010,
        Pressed        = 0b00000100,
        PressedAndDown = 0b00000110,
        PressedAndUp   = 0b00000101
    };

private:
    std::vector<Gamepad::Status> buttons_;    // 32 bytes
    std::vector<float>           axes_;       // 32 bytes

    void* joystick_impl_ = nullptr;    // 8 bytes

    int  id_;                         // 4 bytes
    bool enabled_ = false;            // 1 byte
    Hat  hat_     = Hat::Centered;    // 1 bytes

    char filling_[2];    // 2 bytes are added for padding

    // Total : 80 bytes
    // Using buttons[20] would make it slightly better memory wise
    // but it's ok for now
};
}    // namespace corgi
