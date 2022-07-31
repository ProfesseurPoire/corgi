#include <corgi/math/Vec2.h>
#include <corgi/math/Random.h>
#include <corgi/math/MathUtils.h>

using namespace corgi;

// TODO :	I should check which is faster
	//			Taking 2 random values and normalizing
	//			Or getting 1 random value and using cos/sin/tan
Vec2 Vec2::random(const float length)
{
	const float angle = random::range(-math::pi, math::pi);
	return Vec2
	(
		math::cos(angle) * length,
		math::sin(angle) * length
	);
}

Vec2 Vec2::clamp(const Vec2& u, float min, float max)
{
	if (u.length() < min)
		return u.normalized() * min;

	if (u.length() > max)
		return u.normalized() * max;

	return u;
}

Vec2::Vec2(float p_x, float p_y) noexcept
: x(p_x), y(p_y) {};


bool Vec2::operator<(const Vec2& other)const noexcept
{
	return sqrt_length() < other.sqrt_length();
}

Vec2& Vec2::operator=(const Vec2& v)
{
	x = v.x;
	y = v.y;
	return *this;
}

Vec2& Vec2::operator=(Vec2&& v) noexcept
{
	x = v.x;
	y = v.y;
	return *this;
}

Vec2& Vec2::operator+=(const Vec2& v) noexcept
{
	x += v.x;
	y += v.y;
	return  *this;
}

Vec2& Vec2::operator-=(const Vec2& v) noexcept
{
	x -= v.x;
	y -= v.y;
	return *this;
}

Vec2& Vec2::operator*=(const Vec2& v) noexcept
{
	x *= v.x;
	y *= v.y;
	return *this;
}

Vec2& Vec2::operator*=(const float n) noexcept
{
	x *= n;
	y *= n;
	return *this;
}

Vec2& Vec2::operator/=(const float n) noexcept
{
	x /= n;
	y /= n;
	return *this;
}

bool Vec2::operator==(const Vec2& v)const noexcept
{
	return (x == v.x && y == v.y);
}

bool Vec2::operator!=(const Vec2& v)const noexcept
{
	return !operator==(v);
}


Vec2 Vec2::operator+(const Vec2& v)const noexcept
{
	return Vec2(x + v.x, y + v.y);
}

Vec2 Vec2::operator-(const Vec2& v)const noexcept
{
	return Vec2(x - v.x, y - v.y);
}

Vec2 Vec2::operator*(const Vec2& v)const noexcept
{
	return Vec2(x * v.x, y * v.y);
}

Vec2 Vec2::operator*(const float  n) const noexcept
{
	return Vec2(x * n, y * n);
}

Vec2 Vec2::operator/(const float  n) const noexcept
{
	return Vec2(x / n, y / n);
}

Vec2 Vec2::operator-()const noexcept  // Unary operator
{
	return Vec2(-x, -y);
}

[[nodiscard]] Vec2 Vec2::normalized()const
{
	float l = length();
	if (l == 0)
		return Vec2();

	return Vec2(x / l, y / l);
	//return (*this * math::inverse_sqrt(dot(*this))); Not precise enough
}

Vec2 Vec2::lerp(const Vec2& u, const Vec2& v, const float t)noexcept
{
	return u + ((v - u) * t);
}

bool Vec2::segment_intersect(Vec2 a, Vec2 b, Vec2 c, Vec2 d, Vec2& intersection)
{
	// First we need to make sure that AB and CD aren't collinear
	Vec2 ab = b - a;
	Vec2 cd = d - c;

	// if the dot product equals 0, it means a and b are parallel
	// In that case, no collision can ever happen

	const auto determinant = (ab.x * cd.y) - (ab.y * cd.x);

	if (determinant == 0.0f)
		return false;

	// We get the vector's equation
	Vec2 cc = c - a;

	auto t = (cc.x * cd.y - cc.y * cd.x) / determinant;

	if (t < 0 || t > 1)
		return false; // The segments doesn't intersect

	auto u = (cc.x * ab.y - cc.y * ab.x) / determinant;

	if (u < 0 || u > 1)
		return false; // The segments doesn't intersect

	intersection = a + ab * t;
	return true;
}

float Vec2::dot(const Vec2& v) const noexcept
{
	return x * v.x + y * v.y;
}

bool Vec2::are_orthogonal(const Vec2& v) const noexcept
{
	return dot(v) == 0;
}

float Vec2::angle(const Vec2& v)const
{
	return  math::acosf(this->dot(v) / (this->length() * v.length()));
}

bool Vec2::are_collinear(const Vec2& v)const noexcept
{
	return (x * v.y - v.x * y == 0.0f);
}

float* Vec2::array()noexcept { return &x; }

const float* Vec2::array()const noexcept
{
	return &x;
}

float Vec2::sqrt_length() const noexcept
{
	return (y * y) + (x * x);
}

void Vec2::normalize()
{
	float l = length();
	if (l == 0.0f)
	{
		x = 0.0f;
		y = 0.0f;
		return;
	}

	x = x / l;
	y = y / l;

	//*this *= math::inverse_sqrt(dot(*this));
}

float Vec2::length() const noexcept
{
	return math::sqrtf(sqrt_length());
}

Vec2 Vec2::orthogonal_vector(const Vec2& u)const noexcept
{
	return Vec2(-u.y, u.x);
}

Vec2 Vec2::orthogonal_vector()const noexcept
{
	return Vec2(-y, x).normalized();
}

float Vec2::angle()const noexcept
{
	Vec2 normal = normalized();
	float angle = math::asin(normal.y);

	if (normal.x < 0 && normal.y > 0.0f)
		angle = math::atan(1.0f) * 4.0f - angle;

	if (normal.x < 0 && normal.y < 0.0f)
		angle = -(math::atan(1.0f) * 4.0f) - angle;

	return angle;
}

Vec2 Vec2::range(float range_angle)const
{
	float current_angle = angle();
	current_angle += random::range(-range_angle, range_angle);

	float a = math::sin(current_angle) * length();
	return Vec2(
		math::cos(current_angle),
		a
	);
}

Vec2 Vec2::zero()
{
	return Vec2(0.f, 0.f);
}

Vec2 Vec2::one()
{
	return Vec2(1.f, 1.f);
}

Vec2 Vec2::left()
{
	return Vec2(-1.f, 0.f);
}

Vec2 Vec2::right()
{
	return Vec2(1.f, 0.f);
}

Vec2 Vec2::up()
{
	return Vec2(0.0f, 1.0f);
}

Vec2 Vec2::down()
{
	return Vec2(0.0f, -1.0f);
}

