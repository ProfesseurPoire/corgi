#pragma once

#include <corgi/math/MathUtils.h>

namespace corgi
{
	namespace detail
	{
		/*
		 * @brief	Represents a mathematical 2 dimensional vector. Mainly used
		 *			for geometric operations
		 *
		 *			Template parameter can only be of arithmetic type
		 */
		template<class ValueType>
		class Vec2
		{
		public:

			/*
			 * @brief	Make sure parameter u's length is between min and max
			 *			values. If not, returns multiply the current normalized
			 *			vector with either min or max, depending on which is the
			 *			closest
			 */
			static Vec2 clamp(const Vec2& u, ValueType min, ValueType max)
			{
				if (u.length() < min)
					return u.normalized() * min;

				if (u.length() > max)
					return u.normalized() * max;

				return u;
			}

			// Static Variables

			static const Vec2 one;
			static const Vec2 left;
			static const Vec2 right;
			static const Vec2 up;
			static const Vec2 down;

			//Constructors

			~Vec2() = default;
			constexpr Vec2() noexcept = default;
			constexpr explicit Vec2(ValueType x, ValueType y = 0) noexcept : x(x), y(y) {};

			constexpr Vec2(const Vec2& v) :x(v.x), y(v.y) {}
			constexpr Vec2(Vec2&& v) noexcept : x(v.x), y(v.y) {}

			// Assignment Operators

			bool operator<(const Vec2& other)const noexcept
			{
				return sqrt_length() < other.sqrt_length();
			}

			constexpr Vec2& operator=(const Vec2& v)
			{
				x = v.x;
				y = v.y;
				return *this;
			}

			constexpr Vec2& operator=(Vec2&& v) noexcept
			{
				x = v.x;
				y = v.y;
				return *this;
			}

			constexpr Vec2& operator+=(const Vec2& v) noexcept
			{
				x += v.x;
				y += v.y;
				return  *this;
			}

			constexpr Vec2& operator-=(const Vec2& v) noexcept
			{
				x -= v.x;
				y -= v.y;
				return *this;
			}

			constexpr Vec2& operator*=(const Vec2& v) noexcept
			{
				x *= v.x;
				y *= v.y;
				return *this;
			}

			constexpr Vec2& operator*=(const ValueType n) noexcept
			{
				x *= n;
				y *= n;
				return *this;
			}

			constexpr Vec2& operator/=(const ValueType n) noexcept
			{
				x /= n;
				y /= n;
				return *this;
			}

			// Comparison Operators

			constexpr bool operator==(const Vec2& v)const noexcept
			{
				return (x == v.x && y == v.y);
			}

			constexpr bool operator!=(const Vec2& v)const noexcept
			{
				return !operator==(v);
			}

			// Arithmetic Operators

			constexpr Vec2 operator+(const Vec2& v)const noexcept
			{
				return Vec2(x + v.x, y + v.y);
			}

			constexpr Vec2 operator-(const Vec2& v)const noexcept
			{
				return Vec2(x - v.x, y - v.y);
			}

			constexpr Vec2 operator*(const Vec2& v)const noexcept
			{
				return Vec2(x * v.x, y * v.y);
			}

			constexpr Vec2 operator*(const ValueType n) const noexcept
			{
				return Vec2(x * n, y * n);
			}

			constexpr Vec2 operator/(const ValueType n) const noexcept
			{
				return Vec2(x / n, y / n);
			}

			constexpr Vec2 operator-()const noexcept  // Unary operator
			{
				return Vec2(-x, -y);
			}

			// Functions



				/*
				 * @brief	Returns the current vector with a length equals to 1.0f
				 *			Returns a zero vector if the vector could not be
				 *			normalized
				 */
			[[nodiscard]] Vec2 normalized()const
			{
				float l = length();
				if (l == 0)
					return Vec2();

				return Vec2(x / l, y / l);
				//return (*this * math::inverse_sqrt(dot(*this))); Not precise enough
			}

			/*!
			* @brief	Returns a point located on the [u,v] line segment
			*			This function returns the result of the following equation :
			*			\f$  \vec{u} + (\vec{u}-\vec{v}) times{ValueType} \f$
			*
			* @param u  First point of the line segment [u,v]
			* @param v	Second point of the line segment [u,v]
			* @param t  Parameter used to locate a point on the [u,v] line segment.
			*			Clamped to the [0.0f;1.0f] range
			*/
			static constexpr Vec2 lerp(const Vec2& u, const Vec2& v, const float t)noexcept
			{
				return u + ((v - u) * t);
			}

			constexpr static bool segment_intersect(Vec2 a, Vec2 b,
				Vec2 c, Vec2 d,
				Vec2& intersection)
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
			

			/*!
			 * @brief      Returns the dot product of two vectors
			 *
			 * ### Definition
			 *
			 * The dot product of vectors u and v is defined by the following equation :
			 * \f$ u.v = ||\vec{u}||\times||\vec{v}||\cos(\theta) \f$
			 *
			 * But it can also be computed using this equation :
			 * \f$  ((u_x \times v_x) + (u_y \times v_y)) \f$
			 *
			 * ### Properties
			 *
			 * - **Co-linearity** If the the dot product absolute value between the u and v vectors
			 * is equals to the product of their length, that means u and v are collinear
			 * \f$ |\vec{u}.\vec{v}| = v\times u \f$
			 *
			 * - **Orthogonality** Two vectors are orthogonal if their dot product is equal to zero.
			 * - **Angle** We can use the dot product equation to find the angle between u and v
			 * ( This is actually what is done in the "angle" static function )
			 * \f$  \cos(\theta) = \frac{\vec{u}.\vec{v}}{ ||\vec{u}||\times||\vec{v}|| } \f$
			 */
			[[nodiscard]] constexpr ValueType dot(const Vec2& v) const noexcept
			{
				return x * v.x + y * v.y;
			}
			/*!
			 * @brief	Returns true if u and v are orthogonal
			 *
			 *			Use the dot product under the hood. If the dot product
			 *			is equals to zero, u and v are orthogonal
			 */
			[[nodiscard]] constexpr bool are_orthogonal(const Vec2& v) const noexcept
			{
				return dot(v) == 0;
			}

			/*!
			* @brief	Returns the angle between u and v
			*
			*			The angle is returned in radian, used the dot product
			*			to find the angle. Use the following equation . This method will assume
			*			that both u and v start at the same origin
			*			\f$  \cos(\theta) = \frac{\vec{u}.\vec{v}}{ ||\vec{u}||\times||\vec{v}|| } \f$
			*/
			[[nodiscard]] constexpr ValueType angle(const Vec2& v)const
			{
				return  math::acosf(this->dot(v) / (this->length() * v.length()));
			}

			/*!
			* @brief	Returns true if u and v are collinear
			*/
			[[nodiscard]] constexpr bool are_collinear(const Vec2& v)const noexcept
			{
				return (x * v.y - v.x * y == 0.0f);
			}

			/*!
			 * @brief   Converts the vector in an array
			 *
			 * @return  Returns a pointer to the first value of the vector
			 */
			constexpr ValueType* array()noexcept { return &x; }

			/*!
			 * @brief   Converts the vector to a C array
			 *
			 * @return  Returns a const pointer to the first vector value
			 */
			[[nodiscard]] constexpr ValueType* array()const noexcept
			{
				return &x;
			}

			/*!
			* @brief  Returns the length without performing the sqrt operation
			* Use this function when you only need to know which vector is longer.
			* By avoiding the sqrt operation, you save up precious CPU cycles
			*/
			[[nodiscard]] constexpr ValueType sqrt_length() const noexcept
			{
				return (y * y) + (x * x);
			}

			/*!
			 * @brief      Returns the length of the vector
			 * @returns    \f$ \sqrt{x^2 + y^2} \f$
			 */
			[[nodiscard]] constexpr ValueType length() const noexcept
			{
				return math::sqrtf(sqrt_length());
			}

			/*!
			* @brief	Normalize the current vector
			*
			*			Reduce the vector's length to 1. Keep it's direction. A warning
			*			though, if the length of the vector is equal to zero, the method will return false
			*			\f$ (u_x / l, u_y / l) \f$
			*
			* @returns	Returns false if the length of the vector is equals to zero, true otherwise
			*/
			constexpr void normalize()
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

			/*!
			 * @brief   Returns a vector "v" orthogonal to "u"
			 *
			 *			This function returns the result of the following equation :
			 *			 \f$ v = (-u_y, u_x) \f$
			 */
			[[nodiscard]] constexpr Vec2 orthogonal_vector(const Vec2& u)const noexcept
			{
				return Vector2(-u.y, u.x);
			}

			/*!
			 * @brief Returns a vector "v" orthogonal to "u"
			 * This function returns the result of the following equation :
			 * \f$ v = (-u_y, u_x) \f$
			 */
			[[nodiscard]] constexpr Vec2 orthogonal_vector()const noexcept
			{
				return Vec2(-y, x).normalized();
			}

			[[nodiscard]] ValueType angle()const noexcept
			{
				Vec2 normal = normalized();
				ValueType angle = math::asin(normal.y);

				if (normal.x < 0 && normal.y >0)
					angle = (math::atan(ValueType(1)) * ValueType(4)) - angle;

				if (normal.x < 0 && normal.y < 0)
					angle = -(math::atan(ValueType(1)) * ValueType(4)) - angle;

				return angle;
			}

			/*!
			 * @brief	Returns a random vector of the same length with it's angle in between
			 *			the angle parameter
			 *
			 *			Warning : this might not be the most optimized thing to do since it
			 *			* by length and sometime you don't need that 
			 */
			[[nodiscard]] Vec2 range(ValueType range_angle)
			{
				ValueType current_angle = angle();
				current_angle += -range_angle + static_cast<ValueType>(::rand()) / (static_cast <ValueType> (RAND_MAX) / (range_angle - -range_angle));
				return Vec2(math::cos(current_angle), math::sin(current_angle))* length();
			}

			//[[nodiscard]] Vec2 range_angle(ValueType min_angle, Value)

			// Variables

			ValueType x = ValueType();
			ValueType y = ValueType();
		};

		template<class T> const Vec2<T> Vec2<T>::one(+1, +1);
		template<class T> const Vec2<T> Vec2<T>::left(-1, +0);
		template<class T> const Vec2<T> Vec2<T>::right(+1, +0);
		template<class T> const Vec2<T> Vec2<T>::up(+0, +1);
		template<class T> const Vec2<T> Vec2<T>::down(+0, -1);
	}
}