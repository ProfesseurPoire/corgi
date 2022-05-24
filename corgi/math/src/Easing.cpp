#include <corgi/math/easing.h>

namespace easing
{

float easeInQuad(const float time)
{
    return time*time;
}

float easeOutQuad(const float time)
{
    return 1.f - (1.f - time) * (1.f - time);
}

}