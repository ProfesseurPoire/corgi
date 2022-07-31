#pragma once

#include <corgi/math/Vec2.h>
#include <corgi/math/Vec3.h>

namespace corgi 
{
	class Transform;

	/*
	 * @brief	Represents a line or line segment. Mainly use for collision
	 *			detection, with Raycast classes
	 *			
	 *			A ray is made of a starting point, a direction vector and a length
	 */
	class Ray
	{
	public:

		Ray() = default;

		/*!
		 * @brief	Builds a new ray by using a and b coordinates as points of the line
		 *			segment AB
		 * 
		 * @param a Coordinates of the first point of the line segment AB
		 * @param b Coordinates of the second point of the line segment AB
		 */
		Ray(const Vec3& a, const Vec3& b);

		/*!
		 * @brief	Builds a new ray going from @p start to @p direction of 
		 *			@p length
		 */
		Ray(const Vec3& start, const Vec3& direction, float length);

		Vec3 direction;
		Vec3 start;
		float length{0.0f};
	};

	// We keep it an aggregate type because it's simpler that way
	struct Ray2D
	{
		Vec2 direction;
		Vec2 start;
		float length{ 0.0f };
	};
}
