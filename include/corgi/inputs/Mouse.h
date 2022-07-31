#pragma once

namespace corgi
{
/*!
	 * @brief	Represents the user's mouse and it's current state
	 *
	 *			Highly manipulated by the main/Window class
	 */
class Mouse
{
    friend class Inputs;
    friend class Window;
    friend class Game;

public:
    // Enums/class

    enum class Cursor
    {
        Arrow,
        Wait,
        Crosshair,
        WaitArrow,
        SizeNWSE,
        SizeNESW,
        SizeWE,
        SizeNS,
        SizeAll,
        No,
        Hand
    };

    enum class Button : unsigned
    {
        Left   = 0,
        Middle = 1,
        Right  = 2
    };

    struct MousePoint
    {
        int x = 0;
        int y = 0;
    };

    struct DragEvent
    {
        int    x = 0;
        int    y = 0;
        Button button_;
    };

    // Lifecycle

    Mouse();
    ~Mouse()                  = default;
    Mouse(const Mouse& mouse) = delete;
    Mouse(Mouse&& mouse)      = delete;

    Mouse& operator=(const Mouse& mouse) = delete;
    Mouse& operator=(Mouse&& mouse)      = delete;

    // Position

    /*!
         * @brief	Returns the mouse x position relative to the screen left top corner
         */
    [[nodiscard]] int x() const;

    /*!
         * @brief	Returns the mouse y position relative to the screen top left corner
         */
    [[nodiscard]] int y() const;

    /*!
         * @brief	Returns the mouse's position during the current frame
         */
    [[nodiscard]] MousePoint position() const noexcept;

    /**
         * @brief   Returns the difference between the current and previous position
         * 
         * @return Vec2i 
         */
    [[nodiscard]] MousePoint delta() const noexcept;

    /*!
         * @brief	Returns the mouse's position during the previous frame
         */
    [[nodiscard]] MousePoint previous_position() const noexcept;

    /*!
         * @brief	Returns the mouse's position on the x coordinate during the previous frame
         */
    [[nodiscard]] int previous_x() const;

    /*!
         * @brief	Returns the mouse's position on the y coordinate during the previous frame
         */
    [[nodiscard]] int previous_y() const;

    /*!
         * @brief	Returns true if the mouse moved during this frame
         */
    [[nodiscard]] bool has_moved() const;

    /*!
         * @brief Returns true if the requested mouse button is held down
         */
    [[nodiscard]] bool is_button_pressed(Button button) const;

    /*!
         * @brief	Returns true if the requested mouse button has been pressed during the current frame
         */
    [[nodiscard]] bool is_button_down(Button button) const;

    /*!
         * @brief	Returns true if the requested mouse button has been released during the current frame
         */
    [[nodiscard]] bool is_button_up(Button button) const;

    /**
         * @brief   Gets the distance traveled by the mouse wheel since the last frame
         * @return  Returns an int copy to the member variable mWheelDelta
         */
    [[nodiscard]] int wheelDelta() const noexcept;

protected:
    void update();

private:
    /**
         * @brief Distance traveled by the wheel during the last update
         */
    int mWheelDelta;

    int x_;
    int y_;

    int previous_x_;
    int previous_y_;

    bool move_event_;
    bool button_event_;

    bool drag_event_;

    bool buttons_[3];
    bool buttons_down_[3];
    bool buttons_up_[3];
};
}    // namespace corgi
