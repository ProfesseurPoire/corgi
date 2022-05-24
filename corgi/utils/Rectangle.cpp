#include "Rectangle.h"
#include <corgi/math/MathUtils.h>

using namespace corgi;

Rectangle::Rectangle(float x, float y, float w, float h)
{
	this->x = x;
	this->y = y;
	this->width = w;
	this->height = h;
}

Rectangle::Rectangle(const Vec2& position, const Vec2& size)
{
	x = position.x;
	y = position.y;
	width = size.x;
	height = size.y;
}

bool Rectangle::contains(float x, float y)
{
	if (x >= this->x && x <= (this->x + this->width))
		return (y >= this->y  && y <= (this->y + this->height));
	return false;
}

bool Rectangle::contains(Vec2 v)
{
	return contains(v.x, v.y);
}

bool Rectangle::intersect(Vec2 a1, Vec2 a2, Vec2& intersection)
{
	Vec2 v1;
	Vec2 v2;
	Vec2 v3;
	Vec2 v4;

	int i1 = 0;
	int i2 = 0;
	int i3 = 0;
	int i4 = 0;

	// REPAIR THAT ONE DAY LOL

	/*i1 += math::segment_intersect(math::Vector2f(x, y), math::Vector2f(x + width, y), a1, a2, v1);
	i2 += math::segment_intersect(math::Vector2f(x, y), math::Vector2f(x, y + height), a1, a2, v2);
	i3 += math::segment_intersect(math::Vector2f(x, y + height), math::Vector2f(x + width, y + height), a1, a2, v3);
	i4 += math::segment_intersect(math::Vector2f(x + width, y), math::Vector2f(x + width, y + height), a1, a2, v4);
*/
	int i = i1 + i2 + i3 + i4;

	float distance = 9999999999.9f;
	float dlol = 0.0f;
	if (i1)
	{
		distance = (v1 - a1).length();
		intersection = v1;
	}

	if (i2)
	{
		dlol = (v2 - a1).length();
		if (dlol < distance)
		{
			distance = dlol;
			intersection = v2;
		}
	}

	if (i3)
	{
		dlol = (v3 - a1).length();
		if (dlol < distance)
		{
			distance = dlol;
			intersection = v3;
		}
	}

	if (i4)
	{
		dlol = (v4 - a1).length();
		if (dlol < distance)
		{
			distance = dlol;
			intersection = v4;
		}
	}

	return (i > 0);
}

bool Rectangle::intersect(const Rectangle& rectangle)
{
	if ((rectangle.x >= x + width)
		|| (rectangle.x + rectangle.width <= x)
		|| (rectangle.y >= y + height)
		|| (rectangle.y + rectangle.height <= y))
		return false;
	else

		return true;
}

// The idea is that is r1 and r2 overlap, r2 will be pushed to the closest point
CollisionDirection Rectangle::ResolveCollision(const Rectangle& r1, Rectangle& r2)
{
	float lx = 0.0f, rx = 0.0f, ty = 0.0f, by = 0.0f;

	lx = r2.width + r2.x - r1.x;
	rx = (r1.x + r1.width) - r2.x;
	ty = (r1.y + r1.height) - r2.y;
	by = r2.height + r2.y - r1.y;

	// now I need to pick the min value from all thoses 4 values

	if (lx < rx && lx < ty && lx < by)
	{
		r2.x = r1.x - r2.width;
		return CollisionDirection::Left;
	}

	if (rx < lx && rx < ty && rx < by)
	{
		r2.x = r1.x + r1.width;
		return CollisionDirection::Right;
	}

	if (ty < lx && ty < rx && ty < by)
	{
		r2.y = r1.y + r1.height;
		return CollisionDirection::Up;
	}

	if (by < lx && by < rx && by < ty)
	{
		r2.y = r1.y - r2.height;
		return CollisionDirection::Down;
	}
	return CollisionDirection::None;
}
