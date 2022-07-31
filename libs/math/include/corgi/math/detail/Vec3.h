#pragma once

#include <corgi/math/MathUtils.h>
#include <corgi/math/detail/Vec2.h>

namespace corgi {
	namespace detail {

			template <class ValueType>
			class Vec3
			{
			public:

				// Static functions

				// TODO : Maybe make a Triangle class for that?
				/// Le triangle est déterminé par les points A, B, C, le point P 
				/// appartient au plan du triangle
				static bool point_in_triangle(const Vec3& A, const Vec3& B, const Vec3& C, const Vec3& P)
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

				/*!
					* @brief  Project the point parameter to the axis parameter
					*/
				static constexpr Vec3 project_on_axis(const Vec3& point, const Vec3& axis)
				{
					return axis.normalized() * point.dot(axis);
				}

				// Find the intersection point between line segments AB and CD
				static bool segment_intersection(const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& d, Vec3& returnvec)
				{
					// Dans un premier temps, on vérifie que les vecteurs u et v de ab et cd ne sont pas colinéaire

					Vec3 u = b - a;
					Vec3 v = d - c;

					float determinant = u.x * v.y - v.x * u.y;

					// using an epsilon value
					if (determinant >= -0.01 && determinant <= -0.01)
						return false;

					// Si le déterminant n'est pas égal à 0, alors on cherche l'équation des 2 droites AB et CD
					Vec3 line1 = line_coefficients(a, b);
					Vec3 line2 = line_coefficients(c, d);

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


				/*!
					* @brief	Compute the coefficients of the line ax+by+c=0
					*/
				static Vec3 line_coefficients(const Vec3& u, const Vec3& v)
				{
					return Vec3(v.y - u.y,
						-(v.x - u.x),
						-(u.x * v.y) + (v.x * u.y));
				}

				// Not exactly sure where this was used :thinking:
				// I think this was mainly used for 
				// Perlin noise generation so, maybe not that useful for now?
				// TODO : Create a texture using the whole perlin process and check if this
				// whole mid_point thing works
				//static Vec3 mid_point(const Vec3& start, const Vec3& end, float displacement_value, int occurence)
				//{
				//	Vec3 direction = end - start;

				//	// Find the middle point
				//	Vec3 mid = start + direction / 2.0f;

				//	// We move the mid point by taking a random value

				//	float val = (displacement_value / occurence) * (((float)(rand() % 1000) / 1000.0f) * 2.0f - 1.0f);

				//	Vec3 normal(-direction.y, direction.x, 0.0f);
				//	normal.normalize();
				//	return (mid + normal * val);
				//}


			// Static variables

				static const Vec3 zero;
				static const Vec3 one;
				static const Vec3 left;
				static const Vec3 right;
				static const Vec3 up;
				static const Vec3 down;
				static const Vec3 forward;
				static const Vec3 backward;

				//Constructors

				
				Vec3() = default;

				//  TODO : Real talk, would a move constructor be faster on the vector3 thingie?

				explicit constexpr Vec3(ValueType x, ValueType y = 0, ValueType z = 0) noexcept
					: x(x), y(y), z(z) {};

				// Totally unsafe 
				Vec3(const float* a) : x(a[0]), y(a[1]), z(a[2]) {}

				constexpr Vec3(const Vec2<ValueType>& v)noexcept : x(v.x), y(v.y), z(0.0f){}
				constexpr Vec3(const Vec3& v) noexcept : x(v.x), y(v.y), z(v.z) {}
				constexpr Vec3(Vec3&& v) noexcept : x(v.x), y(v.y), z(v.z) {}

			// Comparison Operators

				bool operator<(const Vec3& other)const noexcept
				{
					return sqrt_length() < other.sqrt_length();
				}
				
				constexpr bool operator==(const Vec3& v)const noexcept
				{
					return equal(v);
				}

				constexpr bool operator!=(const Vec3& v)const noexcept
				{
					return !operator==(v);
				}
				
				constexpr bool equal(const Vec3& v, ValueType epsilon= 0) const noexcept
				{
					return
						x >= v.x - epsilon && x <= v.x + epsilon &&
						y >= v.y - epsilon && y <= v.y + epsilon &&
						z >= v.z - epsilon && z <= v.z + epsilon;
				}

			// Assignment Operators

				constexpr Vec3& operator=(const Vec3& v) noexcept
				{
					x = v.x;
					y = v.y;
					z = v.z;
					return *this;
				}

				constexpr Vec3& operator=(Vec3&& v) noexcept
				{
					x = v.x;
					y = v.y;
					z = v.z;
					return *this;
				}

				constexpr Vec3& operator+=(const Vec3& v) noexcept
				{
					x += v.x;
					y += v.y;
					z += v.z;
					return  *this;
				}

				constexpr Vec3& operator-=(const Vec3& v) noexcept
				{
					x -= v.x;
					y -= v.y;
					z -= v.z;
					return *this;
				}

				constexpr Vec3& operator*=(const Vec3& v) noexcept
				{
					x *= v.x;
					y *= v.y;
					z *= v.z;
					return *this;
				}

				constexpr Vec3& operator*=(const ValueType n) noexcept
				{
					x *= n;
					y *= n;
					z *= n;
					return *this;
				}

				constexpr Vec3& operator/=(const ValueType n) noexcept
				{
					x /= n;
					y /= n;
					z /= n;
					return *this;
				}

			// Arithmetics Operators

				constexpr Vec3 operator+(const Vec3& v)const noexcept
				{
					return Vec3(x + v.x, y + v.y, z+v.z);
				}

				constexpr Vec3 operator-(const Vec3& v)const noexcept
				{
					return Vec3(x - v.x, y - v.y, z-v.z);
				}

				constexpr Vec3 operator*(const Vec3& v)const noexcept
				{
					return Vec3(x * v.x, y * v.y, z* v.z);
				}

				constexpr Vec3 operator*(const ValueType n) const noexcept
				{
					return Vec3(x * n, y * n, z*n);
				}

				constexpr Vec3 operator/(const ValueType n) const noexcept
				{
					return Vec3(x / n, y / n, z/n);
				}

				constexpr Vec3 operator-()const noexcept  // Unary operator
				{
					return Vec3(-x, -y, -z);
				}

			// Functions

				
				
				// Returns a Vec2 using the Vec3's x and y value
				[[nodiscard]] Vec2<ValueType> xy()const{return Vec2<ValueType>(x, y);}

				[[nodiscard]] Vec3 normalized()const
				{
					float l = length();

					if (l == 0)
						return Vec3();

					return Vec3(x / l, y / l, z/l);

					// the length thing actually goes slightly faster than
					// the inverse sqrt tricks ... wth?
					//return *this * math::inverse_sqrt(dot(*this));
				}

				void normalize() noexcept
				{
					*this *= math::inverse_sqrt(dot(*this));
				}

				[[nodiscard]] constexpr ValueType sqrt_length()const noexcept
				{
					return((x * x) + (y * y) + (z * z));
				}

				[[nodiscard]] ValueType length()const noexcept
				{
					return math::sqrtf(sqrt_length());
				}

				/*!
					* @brief  Compute the cross product of vectors $\f \vec{v} \vec{u} $\f
					*/
				[[nodiscard]] constexpr Vec3 cross(const Vec3& v)const noexcept
				{
					return Vec3
					(
						(y * v.z) - (z * v.y),
						(z * v.x) - (x * v.z),
						(x * v.y) - (y * v.x)
					);
				}

				[[nodiscard]] constexpr ValueType dot(const Vec3& v) const noexcept
				{
					return (x * v.x) + (y * v.y) + (z * v.z);
				}

				// Variables

				ValueType x = ValueType();
				ValueType y = ValueType();
				ValueType z = ValueType();
			};


			template<class ValueType> const Vec3<ValueType> Vec3<ValueType>::zero(+0, +0, +0);
			template<class ValueType> const Vec3<ValueType> Vec3<ValueType>::one(+1, +1, +1);
			template<class ValueType> const Vec3<ValueType> Vec3<ValueType>::left(-1, +0, +0);
			template<class ValueType> const Vec3<ValueType> Vec3<ValueType>::right(+1, +0, +0);
			template<class ValueType> const Vec3<ValueType> Vec3<ValueType>::up(+0, +1, +0);
			template<class ValueType> const Vec3<ValueType> Vec3<ValueType>::down(+0, -1, +0);
			template<class ValueType> const Vec3<ValueType> Vec3<ValueType>::forward( +0, +0, +1);
			template<class ValueType> const Vec3<ValueType> Vec3<ValueType>::backward(+0, +0, -1);
		}
	}
