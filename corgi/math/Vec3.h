#pragma once

#include <corgi/math/Vec2.h>

namespace corgi
{
	class Vec3
	{
	public:

		// Static functions

		// TODO : Maybe make a Triangle class for that?
		/// Triangle is determined by points A B C and the P Point is within
		/// the triangle's plane
		static bool point_in_triangle(const Vec3& A, const Vec3& B, const Vec3& C, const Vec3& P);

		/*!
			* @brief  Project the point parameter to the axis parameter
			*/
		static Vec3 project_on_axis(const Vec3& point, const Vec3& axis);

		// Find the intersection point between line segments AB and CD
		static bool segment_intersection(const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& d, Vec3& returnvec);

		/*!
		 * @brief	Compute the coefficients of the line ax+by+c=0
		 */
		static Vec3 line_coefficients(const Vec3& u, const Vec3& v);

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

		/**
		 * @brief Returns the zero vector
		 * 
		 * @return Vec3 Value is (0.f, 0.f, 0.f)
		 */
		static Vec3 zero();

		/**
		 * @brief Returns the 1 vector
		 * 
		 * @return Vec3 Value is (1.f, 1.f, 1.f)
		 */
		static Vec3 one();

		/**
		 * @brief Returns the left unit vector
		 * 
		 * @return Vec3 Value is (-1.f, 0.f, 0.f)
		 */
		static Vec3 left();

		/**
		 * @brief Returns the right unit vector
		 * 
		 * @return Vec3 Value is (1.f, 0.f, 0.f)
		 */
		static Vec3 right();

		/**
		 * @brief Returns the up unit vector
		 * 
		 * @return Vec3 Value is (0.f, 1.f, 0.f)
		 */
		static Vec3 up();

		/**
		 * @brief Returns the down unit vector
		 * 
		 * @return Vec3 Value is (0.f, -1.f, 0.f)
		 */
		static Vec3 down();

		/**
		 * @brief Returns the forward unit vector
		 * 
		 * @return Vec3 Value is (0.f, 0.f, -1.f)
		 */
		static Vec3 forward();

		/**
		 * @brief Returns the bacward unit vector
		 * 
		 * @return Vec3 Value is (0.f, 0.f, 1.f)
		 */
		static Vec3 backward();

		//Constructors


		Vec3() = default;

		explicit Vec3(float p_x, float p_y = 0, float p_z = 0) noexcept
			: x(p_x), y(p_y), z(p_z) {}

		// Totally unsafe 
		Vec3(const float* a) : x(a[0]), y(a[1]), z(a[2]) {}

		Vec3(const Vec2& v)noexcept : x(v.x), y(v.y), z(0.0f) {}
		Vec3(const Vec3& v) noexcept : x(v.x), y(v.y), z(v.z) {}
		Vec3(Vec3&& v) noexcept : x(v.x), y(v.y), z(v.z) {}

		// Comparison Operators

		bool operator<(const Vec3& other)const noexcept;

		bool operator==(const Vec3& v)const noexcept;
		bool operator!=(const Vec3& v)const noexcept;

		bool equal(const Vec3& v, float epsilon = 0) const noexcept;

		// Assignment Operators

		Vec3& operator=(const Vec3& v) noexcept;
		Vec3& operator=(Vec3&& v) noexcept;
		Vec3& operator+=(const Vec3& v) noexcept;
		Vec3& operator-=(const Vec3& v) noexcept;
		Vec3& operator*=(const Vec3& v) noexcept;
		Vec3& operator*=(float n) noexcept;
		Vec3& operator/=(float n) noexcept;

		// Arithmetics Operators

		Vec3 operator+(const Vec3& v)const noexcept;
		Vec3 operator-(const Vec3& v)const noexcept;
		Vec3 operator*(const Vec3& v)const noexcept;
		Vec3 operator*(float n) const noexcept;
		Vec3 operator/(float n) const noexcept;

		Vec3 operator-()const noexcept;  // Unary operator

		// Functions

		// Returns a Vec2 using the Vec3's x and y value
		[[nodiscard]] Vec2 xy()const { return Vec2(x, y); }

		[[nodiscard]] Vec3 normalized()const;

		void normalize() noexcept;

		[[nodiscard]] float sqrt_length()const noexcept;

		[[nodiscard]] float length()const noexcept;

		/*!
		 * @brief  Compute the cross product of vectors \f$ \vec{v} \vec{u} \f$
		 */
		[[nodiscard]] Vec3 cross(const Vec3& v)const noexcept;

		[[nodiscard]] float dot(const Vec3& v) const noexcept;

		// Variables

		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	};
}
