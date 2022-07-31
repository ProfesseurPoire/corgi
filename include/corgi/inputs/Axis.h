#pragma once

#include <corgi/containers/Vector.h>
#include <corgi/inputs/Gamepad.h>
#include <corgi/inputs/Keys.h>

namespace corgi
{
    class Inputs;

    class Axis
    {

    public:
        // Lifecycle

        Axis(const Inputs& inputs, Key positive, Key negative);
        Axis(const Axis&)                = delete;
        Axis(Axis&&) noexcept            = delete;
        Axis& operator=(const Axis&)     = delete;
        Axis& operator=(Axis&&) noexcept = delete;

        // Functions

        /**
         * @brief Adds 2 hat value, one positive, one negative to the axis
         *  
         *          It means that the axis will return +1 if the positive hat is triggered
         *          and -1 if the negative hat is triggered
         * 
         *          We can have more than one hat 
         * 
         * @param positive 
         * @param negative 
         */
        void add_hat_values(Gamepad::Hat positive, Gamepad::Hat negative);

        void set_axis(int id);

        float value() const;

    private:
        [[nodiscard]] bool is_hat_triggered(const Vector<Gamepad::Hat>& hat_values) const;
        [[nodiscard]] bool is_positive_hat_triggered() const;
        [[nodiscard]] bool is_negative_hat_triggered() const;

        const Inputs& inputs_;    // 4

        int axe_id_ = -1;    // 4

        // The axis will take a positive value when the key is positive
        Key positive_;    // 4

        // the axis will take a negative value when the negative key is pressed
        Key negative_;    // 4

        // We can have more than one "hat" value that triggers the positive
        // or negative axis value
        Vector<Gamepad::Hat> positive_hat_;    // 12
        Vector<Gamepad::Hat> negative_hat_;    // 12
    };
}    // namespace corgi
