#pragma once

#include <corgi/math/Vec2.h>

namespace corgi
{
	enum class CollisionDirection
	{
		Up,
		Down,
		Left,
		Right,
		None
	};

	struct Rectangle
	{
		static CollisionDirection ResolveCollision(const Rectangle& r1, Rectangle& r2);

		Rectangle(float x = 0.0f, float y = 0.0f, float width = 0.0f, float height = 0.0f);
		Rectangle(const Vec2& position, const Vec2& size);

		bool contains(float x, float y);
		bool contains(Vec2 position);

		bool intersect(const Vec2 a1, Vec2 a2, Vec2& intersection);
		bool intersect(const Rectangle& r);

		float x, y, width, height;
	};
}