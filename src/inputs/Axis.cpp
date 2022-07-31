#include <corgi/inputs/Axis.h>
#include <corgi/inputs/Inputs.h>

using namespace corgi;

bool Axis::is_hat_triggered(const Vector<Gamepad::Hat>& hat_values) const
{
    if(!inputs_.is_gamepad_connected(0))
        return false;

    return hat_values.contains(inputs_.gamepad(0).hat());
}

float Axis::value() const
{
    if(axe_id_ != -1)
    {
        const auto threshold = 0.3f;

        if(inputs_.is_gamepad_connected(0))
        {
            if(!(inputs_.gamepad(0).axis(axe_id_) < threshold) &&
               (inputs_.gamepad(0).axis(axe_id_) > -threshold))
            {
                return inputs_.gamepad(0).axis(axe_id_);
            }
        }
    }

    if(inputs_.keyboard().key(positive_) ||
       (!positive_hat_.empty() && is_positive_hat_triggered()))
    {
        return 1.0f;
    }

    if(inputs_.keyboard().key(negative_) ||
       (!negative_hat_.empty() && is_negative_hat_triggered()))
    {
        return -1.0f;
    }

    return 0.0f;
}

void Axis::set_axis(int id)
{
    axe_id_ = id;
}

[[nodiscard]] bool Axis::is_positive_hat_triggered() const
{
    return is_hat_triggered(positive_hat_);
}

[[nodiscard]] bool Axis::is_negative_hat_triggered() const
{
    return is_hat_triggered(negative_hat_);
}

void Axis::add_hat_values(Gamepad::Hat positive, Gamepad::Hat negative)
{
    positive_hat_.push_back(positive);
    negative_hat_.push_back(negative);
}

Axis::Axis(const Inputs& inputs, Key positive, Key negative)
    : inputs_(inputs)
    , positive_(positive)
    , negative_(negative)
{
}
