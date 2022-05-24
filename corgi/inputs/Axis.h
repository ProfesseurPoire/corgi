#pragma once

#include <corgi/containers/Vector.h>

#include <corgi/inputs/Keys.h>
#include <corgi/inputs/Gamepad.h>

namespace corgi
{
    class Inputs;

    class Axis
    {
        const Inputs& inputs_;
        
    public:

    // Lifecycle

        Axis(const Inputs& inputs , Key positive, Key negative);
        Axis(const Axis&)       = delete;
        Axis(Axis&&) noexcept   = delete;
        Axis& operator=(const Axis&) = delete;
        Axis& operator=(Axis&&)noexcept = delete;

    // Functions

        void add_hat_values(Gamepad::Hat positive, Gamepad::Hat negative);

        void set_axis(int id);

        float value() const;

    private:

        [[nodiscard]] bool is_hat_triggered(const Vector<Gamepad::Hat>& hat_values)const;
        [[nodiscard]] bool is_positive_hat_triggered() const ;
        [[nodiscard]] bool is_negative_hat_triggered() const;

        int axe_id_ = -1;
        
        // The axis will take a positive value when the key is positive
        Key positive_;

        // the axis will take a negative value when the negative key is pressed
        Key negative_;

        bool use_hat_ = false;
        
        Vector<Gamepad::Hat> positive_hat_;
        Vector<Gamepad::Hat> negative_hat_;
    };
}
