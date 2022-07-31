#include <corgi/math/Random.h>

#include <random>

namespace corgi::random
{
    // Check my obsidian for why we use this instead of just doing rand
    static std::random_device rd;
    // By default we simply initialize the generator with
    // the random_device
    static std::mt19937 mt(rd());

    void initialize(const int seed) { mt.seed(static_cast<unsigned int>(seed)); }

    int range(const int min, const int max)
    {
        std::uniform_int_distribution ud(min, max);
        return ud(mt);
    }

    unsigned uinteger() { return mt(); }

    int range(const int max)
    {
        std::uniform_int_distribution ud(0, max);
        return ud(mt);
    }

    float range(const float min, const float max)
    {
        std::uniform_real_distribution ud(min, max);
        return ud(mt);
    }

    float range(const float max)
    {
        std::uniform_real_distribution ud(0.0f, max);
        return ud(mt);
    }

    float real_value()
    {
        std::uniform_real_distribution<float> ud;
        return ud(mt);
    }

    //Vec3 random_vec3(const float length)
    //{
    //	// We simply generate 2 random angles between -pi and pi
    //	// and plot the point on the sphere
    //	const std::uniform_real_distribution ud
    //	(
    //		-math::pi,
    //		math::pi
    //	);

    //	// angles
    //	const float u = ud(mt);
    //	const float v = ud(mt);

    //	return Vec3(
    //		length * cos(u) * sin(v),
    //		length * sin(u) * sin(v),
    //		length * cos(v)
    //		);
    //}
}    // namespace corgi::random
