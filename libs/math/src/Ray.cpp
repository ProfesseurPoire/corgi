#include <corgi/math/Ray.h>

namespace corgi
{
	Ray::Ray(const Vec3& a, const Vec3& b)
	{
		direction = (b - a).normalized();
		direction.normalize();
		start = a;
		length = (b - a).length();
	}

	Ray::Ray(const Vec3& start, const Vec3& direction, float length)
		: direction(direction), start(start), length(length)
	{}
}
