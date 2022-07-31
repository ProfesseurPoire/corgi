#include <corgi/math/MathUtils.h>

#include <cmath>
#include <limits>

bool corgi::math::in_range(const float value, const float left, const float right)
{
    return ((value >= left) && (value <= right));
}

int corgi::math::max(int a, int b)
{
    return (a < b) ? b : a;
}

float corgi::math::max(float a, float b)
{
    return (a < b) ? b : a;
}

int corgi::math::min(int a, int b)
{
    return (a < b) ? a : b;
}

float corgi::math::min(float a, float b)
{
    return (a < b) ? a : b;
}

float corgi::math::sign(const float value)
{
    if(value >= 0.0f)
    {
        return 1.0f;
    }
    return -1.0f;
}

float corgi::math::clamp(const float value, const float min, const float max) noexcept
{
    if(value < min)
        return min;

    if(value > max)
        return max;

    return value;
}

int corgi::math::clamp(const int value, const int min, const int max)
{
    if(value < min)
        return min;

    if(value > max)
        return max;

    return value;
}

int corgi::math::round(const float value)
{
    return static_cast<int>(std::round(value));
}

float corgi::math::floor(float value)
{
    return std::floor(value);
}

float corgi::math::ceil(float val)
{
    return std::ceil(val);
}

double corgi::math::bernstein_polynomial(const int n, const int i, const double t)
{
    const auto dn = static_cast<double>(n);
    const auto di = static_cast<double>(i);

    auto       numerator   = factorial(dn);
    const auto denominator = (factorial(di) * factorial(dn - di));
    numerator              = numerator / denominator;

    numerator *= ::pow(t, di);
    numerator *= ::pow(1.0 - t, dn - di);
    return numerator;
}

double corgi::math::riesenfeld_polynomial(const int i, const int n, const double t)
{
    const auto dn = static_cast<double>(n);
    const auto di = static_cast<double>(i);

    auto sum = 0.0;

    for(auto k = 0; k <= (dn - di); k++)
    {
        const auto dk          = static_cast<double>(k);
        const auto numerator   = ::pow(-1.0, dk) * ::pow(t + dn - di - dk, dn);
        const auto denominator = factorial(dk) * factorial(dn - dk + 1.0);
        sum += numerator / denominator;
    }
    return (dn + 1) * sum;
}

float corgi::math::inverse_sqrt(const float number)
{
    // I have no idea the fuck is going on, probably something I took
    // from John Carmack or whatever, the famoso inverse fast sqrt thing
    long        i;
    float       x2, y;
    const float threehalfs = 1.5F;
    x2                     = number * 0.5F;
    y                      = number;
    i                      = *(long*)&y;
    i                      = 0x5f3759df - (i >> 1);
    y                      = *(float*)&i;
    y                      = y * (threehalfs - (x2 * y * y));
    return y;
}

float corgi::math::pow(const float value, const float n)
{
    return ::powf(value, n);
}

float corgi::math::sqrtf(const float value)
{
    return ::sqrtf(value);
}

float corgi::math::acosf(const float radian)
{
    return ::acos(radian);
}

float corgi::math::asinf(const float radian)
{
    return ::asin(radian);
}

float corgi::math::absf(const float value)
{
    if(value < 0.0f)
        return value * -1.0f;
    return value;
}

float corgi::math::cos(const float radian)
{
    return ::cos(radian);
}

double corgi::math::cos(double radian)
{
    return ::cos(radian);
}

float corgi::math::sin(float radian)
{
    return ::sin(radian);
}

double corgi::math::sin(double radian)
{
    return ::sin(radian);
}

float corgi::math::sinf(const float radian)
{
    return ::sinf(radian);
}

float corgi::math::atan(const float radian)
{
    return ::atan(radian);
}

double corgi::math::atan(const double radian)
{
    return ::atan(radian);
}

float corgi::math::asin(const float radian)
{
    return ::asin(radian);
}

double corgi::math::asin(const double radian)
{
    return ::asin(radian);
}
