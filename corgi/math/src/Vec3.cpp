#include <corgi/math/Vec3.h>
#include <corgi/math/MathUtils.h>

using namespace corgi;

bool Vec3::point_in_triangle(const Vec3& A, const Vec3& B, const Vec3& C, const Vec3& P)
{
	// Le nom de l'algorithm : Barycentric Coordinates
	// L'objectif est en gros de caractérisé un point dans le triangle à l'aide de 2 vecteurs
	// u et v. P = u*(C-A) + v*(B-A)
	// Le but est de déterminé la valeur de u et de v en fonction du point P. si u et v > 0 et
	//u+v < 1, alors on se trouve dans le triangle
	Vec3 u = B - A;
	Vec3 v = C - A;
	Vec3 w = P - A;

	// On fait le produit vectoriel 
	Vec3 vCrossW = v.cross(w);
	Vec3 vCrossU = v.cross(u);

	// On teste le signe de r
	if (vCrossW.dot(vCrossU) < 0)
		return false;

	Vec3 uCrossW = u.cross(w);
	Vec3 uCrossV = u.cross(v);

	if (uCrossW.dot(uCrossV) < 0)
		return false;

	float denom = uCrossV.length();
	float r = vCrossW.length() / denom;
	float t = uCrossW.length() / denom;

	return (r + t <= 1);
}


Vec3 Vec3::project_on_axis(const Vec3& point, const Vec3& axis)
{
	return axis.normalized() * point.dot(axis);
}

bool segment_intersection(const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& d, Vec3& returnvec)
{
	// Dans un premier temps, on vérifie que les vecteurs u et v de ab et cd ne sont pas colinéaire

	Vec3 u = b - a;
	Vec3 v = d - c;

	float determinant = u.x * v.y - v.x * u.y;

	// using an epsilon value
	if (determinant >= -0.01 && determinant <= -0.01)
		return false;

	// Si le déterminant n'est pas égal à 0, alors on cherche l'équation des 2 droites AB et CD
	Vec3 line1 = Vec3::line_coefficients(a, b);
	Vec3 line2 = Vec3::line_coefficients(c, d);

	float xx = line1.y * line2.z - line2.y * line1.z;
	float yy = line2.x * line1.z - line1.x * line2.z;
	float ww = line1.x * line2.y - line2.x * line1.y;

	returnvec = Vec3(xx / ww, yy / ww, 0.0f);

	float coefa = ((returnvec - a).length()) / u.length();
	float coefb = ((returnvec - c).length()) / v.length();

	if (coefa >= 0 && coefa <= 1)
		if (coefb >= 0 && coefb <= 1)
			return true;
	return false;
}

Vec3 Vec3::line_coefficients(const Vec3& u, const Vec3& v)
{
	return Vec3(v.y - u.y,
		-(v.x - u.x),
		-(u.x * v.y) + (v.x * u.y));
}

bool Vec3::operator<(const Vec3& other)const noexcept
{
	return sqrt_length() < other.sqrt_length();
}

bool Vec3::operator==(const Vec3& v)const noexcept
{
	return equal(v);
}

bool Vec3::operator!=(const Vec3& v)const noexcept
{
	return !operator==(v);
}

bool Vec3::equal(const Vec3& v, float epsilon) const noexcept
{
	return
		x >= v.x - epsilon && x <= v.x + epsilon &&
		y >= v.y - epsilon && y <= v.y + epsilon &&
		z >= v.z - epsilon && z <= v.z + epsilon;
}

Vec3& Vec3::operator=(const Vec3& v) noexcept
{
	x = v.x;
	y = v.y;
	z = v.z;
	return *this;
}

Vec3& Vec3::operator=(Vec3&& v) noexcept
{
	x = v.x;
	y = v.y;
	z = v.z;
	return *this;
}

Vec3& Vec3::operator+=(const Vec3& v) noexcept
{
	x += v.x;
	y += v.y;
	z += v.z;
	return  *this;
}

Vec3& Vec3::operator-=(const Vec3& v) noexcept
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

Vec3& Vec3::operator*=(const Vec3& v) noexcept
{
	x *= v.x;
	y *= v.y;
	z *= v.z;
	return *this;
}

Vec3& Vec3::operator*=(const float n) noexcept
{
	x *= n;
	y *= n;
	z *= n;
	return *this;
}

Vec3& Vec3::operator/=(const float n) noexcept
{
	x /= n;
	y /= n;
	z /= n;
	return *this;
}

Vec3 Vec3::operator+(const Vec3& v)const noexcept
{
	return Vec3(x + v.x, y + v.y, z + v.z);
}

Vec3 Vec3::operator-(const Vec3& v)const noexcept
{
	return Vec3(x - v.x, y - v.y, z - v.z);
}

Vec3 Vec3::operator*(const Vec3& v)const noexcept
{
	return Vec3(x * v.x, y * v.y, z * v.z);
}

Vec3 Vec3::operator*(const float n) const noexcept
{
	return Vec3(x * n, y * n, z * n);
}

Vec3 Vec3::operator/(const float n) const noexcept
{
	return Vec3(x / n, y / n, z / n);
}

Vec3 Vec3::operator-()const noexcept  // Unary operator
{
	return Vec3(-x, -y, -z);
}

[[nodiscard]] Vec3 Vec3::normalized()const
{
	float l = length();

	if (l == 0)
		return Vec3();

	return Vec3(x / l, y / l, z / l);

	// the length thing actually goes slightly faster than
	// the inverse sqrt tricks ... wth?
	//return *this * math::inverse_sqrt(dot(*this));
}

void Vec3::normalize() noexcept
{
	*this *= math::inverse_sqrt(dot(*this));
}

float Vec3::sqrt_length()const noexcept
{
	return((x * x) + (y * y) + (z * z));
}

float Vec3::length()const noexcept
{
	return math::sqrtf(sqrt_length());
}

[[nodiscard]] Vec3 Vec3::cross(const Vec3& v)const noexcept
{
	return Vec3
	(
		(y * v.z) - (z * v.y),
		(z * v.x) - (x * v.z),
		(x * v.y) - (y * v.x)
	);
}

[[nodiscard]] float Vec3::dot(const Vec3& v) const noexcept
{
	return (x * v.x) + (y * v.y) + (z * v.z);
}

Vec3 Vec3::zero()		{ return Vec3(0.f,	0.f,	0.f); };
Vec3 Vec3::one()		{ return Vec3(1.f,	1.f,	1.f); };
Vec3 Vec3::left()		{ return Vec3(-1.f, 0.f,	0.f); };
Vec3 Vec3::right()		{ return Vec3(1.f,	0.f,	0.f); };
Vec3 Vec3::up()			{ return Vec3(0.f,	1.f,	0.f); };
Vec3 Vec3::down()		{ return Vec3(0.f,	-1.f,	0.f); };
Vec3 Vec3::forward()	{ return Vec3(0.f,	0.f,	-1.f); };
Vec3 Vec3::backward()	{ return Vec3(0.f,	0.f,	1.f); };
