#pragma once

#include <corgi/math/Vec2.h>
#include <corgi/math/Vec3.h>
#include <corgi/math/Vec4.h>

// TODO : Make this a template class
namespace corgi
{
    /*!
     *	@brief	4x4 Represents a system of linear equations.
	 *
     *			It's mainly used by opengl for geometric transformations.
     *			Values are stored in column order for opengl but the Matrixf object is
     *			in row order.
     */
	class Matrix
	{
		float _items[16];

	public:

	// Static Methods

		// @brief  The determinant of a Matrixf help to know if Matrixf is invertible
		static float determinant(const Matrix& m) ;

		/*!
			@brief  Compute the determinant of a 3x3 Matrix
			Needed to compute the determinant of a 4x4 Matrix
		*/
		static float determinant(const float a, const float b, const float c, const float d, const float e, const float f, const float g, const float h, const float i) ;

		/*!
			@brief  Compute the determinant of a 2x2 Matrix
			Needed to compute the determinant of a 3x3 Matrix.
		*/
		static float determinant(const float a, const float b, const float c, const float d);

		// @brief  Look at Matrix
		static Matrix look_at(const Vec3& eye, const Vec3& center);

		/*!
			@brief  Returns the result of a translation, rotation and scaling Matrix
			Most 3D application use a transform Matrix for their object
		*/
		static Matrix transform(const Vec3& t, const Vec3& r, const Vec3& s) ;

		/*!
			@brief  Returns a Matrix that modelize a rotation on the x, then y, then z
			axis
			@param x Rotation angle in radian for the x axis
			@param y Rotation angle in radian for the y axis
			@param z Rotation angle in radian for the z axis
		*/
		static Matrix euler_angles(const float x, const float y, const float z);

		/*!
			@brief      Returns a Matrix that can be used to translate a point

			Multiplying the Matrix to a point/vector will have for effect
			to translate it. Imagine we had a point A (0.0, 3.0). If we ask for a
			translation Matrix with 3x as a parameter, mutliplying the vector by the
			Matrix will give us a new point A'(3.0, 3.0)

			@param x	Translation vector's coordinates on the x axis
			@param y	Translation vector's coordinates on the y axis
			@param z	Translation vector's coordinates on the z axis
		*/
		static Matrix translation(const float x, const float y, const float z);

		/*!
			@brief      Returns a Matrix that can be used to translate a point

			Multiplying the Matrix to a point/vector will have for effect
			to translate it. Imagine we had a point A (0.0, 3.0). If we ask for a
			translation Matrix with 3x as a parameter, mutliplying the vector by the
			Matrix will give us a new point A'(3.0, 3.0)

			@param t	Translation vector's coordinates
		*/
		static Matrix translation(const Vec3 & t);

		/*!
			@brief  Creates an orthographic projection Matrix

			An orthographic projection Matrix can be viewed as a rectangular
			volume.

			Every world or view coordinates inside the rectangular volume will
			be transformed to normalized device coordinates.
			Then, every coordinates inside the [-1;+1] NDC cube will be
			displayed by OpenGL while the rest will be clipped out.

			The projection is in a right hand coordinate system, while the
			the NDC cube is in a left hand coordinate system.

							znear         zFar
				^ (y)        +-----------+ top
				|            |           |
			<--* (0,0,0)     *           |
			(z)              |           |
							 +-----------+ bottom

			@param l	Left border of the near plane
			@param r	Right border of the near plane
			@param b	Bottom border of the near plane
			@param t	Top border of the near plane
			@param zn	Distance between the center of the projection
			and the near plane
			@param zf	Distance between the center of the projection
			and the far plane
		*/
		static Matrix ortho(const float l, const float r, const float b, const float t, const float zn, const float zf) ;

		/*!
			@brief  Creates a frustum projection Matrix

			The frustum projection Matrix can be viewed as a truncated
			pyramid.

			Every world or view coordinates inside the pyramid will be transformed
			to normalized device coordinates.
			Then, every coordinates inside the [-1:+1] NDC cube will be displayed
			by OpenGL while the rest will be clipped out.

			The projection is in a right hand coordinate system, while the
			the NDC cube is in a left hand coordinate system.

											zFar
									  ---+
							zNear ---/   |
				^ (y)         ---/       |
				|            |           |
			<--* (0,0,0)     +           |
			(z)              |           |
							  ---\       |
								  ---\   |
									  ---+

			@param left     Left border of the near plane
			@param right    Right border of the near plane
			@param bottom   Bottom border of the near plane
			@param top      Top border of near plane
			@param znear    Distance where the zNear plane start
			@param zfar     Distance where the zFar plane start
		*/
		static Matrix frustum(const float left, const float right, const float bottom, const float top, const float znear, const float zfar);

		// @brief  Creates a Matrix that rotate points on the x axis
		static Matrix rotation_x(const float angle) ;

		// @brief  Creates a Matrix that rotate points on the y axis
		static Matrix rotation_y(const float angle);

		// @brief  Creates a Matrix that rotate points on the z axis
		static Matrix rotation_z(const float angle);

		/*!
			@brief  Creates a scaling Matrix
			Scaling is a linear transformation that enlarge or shrink a coordinate.
			The x value for instance will simply by multiplied by the x scale value
			A scale value of 1 will have no effect.
			@param x Scale value in the x axis
			@param y Scale value for the y axis
			@param z Scale value for the z axis
		*/
		static Matrix scale(const float x, const float y, const float z);

	// Lifecycle 

            
		// @brief  Builds an identity Matrix
		Matrix() ;

		Matrix(float array[16]) ;

		Matrix(float m11, float m12, float m13, float m14,
				float m21, float m22, float m23, float m24,
				float m31, float m32, float m33, float m34,
				float m41, float m42, float m43, float m44);

	// Methods

		/*!
			*	@brief  Returns the inverse Matrix of the current Matrix.
			*
			*	This function returns the inverse Matrix of the current Matrix. If no inverse Matrix could be
			*	computed, the identity Matrix will be returned. Multiplying the current Matrix with it's inverse
			*	will give the identity Matrix (The inverse Matrix give the opposite transformation of the current
			*	Matrix)
			*/
		Matrix inverse()const;

		/*!
			@brief      Make the current Matrix a identity Matrix
			@details    Multiplying a Matrix m by a identity Matrix has no effect. The
			inverse of a identity Matrix is a identity Matrix.
			\f$
			I = \begin{bMatrixf}
				1 & 0 & 0 & 0 \\
				0 & 1 & 0 & 0 \\
				0 & 0 & 1 & 0 \\
				0 & 0 & 0 & 1 \\
				\end{bMatrixf}
			\f$
		*/
		void identity() ;

		// Arithmetics operators

		/*!
			@brief      Compute and returns the result of the multiplication between the current Matrix
			and @ref right Matrix
			@details    Multiplying a Matrix also stack the transformations

			Matrix multiplication example with a 2 dimensional Matrix
			// TODO : Use the real mathematical definition

			n1                          n2
			n3                          n4
			m1 m2   ( m1*n1 ) + ( m2*n3 )       ( m1*n2 )+( m2*n4 )
			m3 m4   ( m3*n1 ) + ( m4*n3 )       ( m3*n2 )+( m4*n4 )
		*/
		Matrix operator*(const Matrix& r)const;

		/*!
			@brief      Mutliply the current Matrix with the vector
			@details    TODO : Add the math behind this Matrix multiplication

			@return     Returns the result of the M*V operation
		*/
		Vec3 operator*(const Vec3& v)const;

		/*!
		 * @brief	Multiplies the current Matrix with the given 2D vector
		 *
		 * @return	Returns the result of the M*V operation
		 */
		Vec2 operator*(const Vec2& v)const;

		/*!
			@brief      Mutliply the current Matrix with the vector
			@details    TODO : Add the math behind this Matrix multiplication
		*/
		Vec4 operator*(const Vec4& v)const;

		Matrix operator/(const float v)const;
		Matrix operator*(const float v)const;

		/*!
		 *	@brief	Returns the Matrix as a float array. Mainly useful
		 *			for opengl stuff
		 */
		[[nodiscard]] const float* data() const
		{
			return _items;
		}

	// Comparison operators

		bool operator==(const Matrix& m)const;
		bool operator!=(const Matrix& m)const;

	// Assignement operators

		Matrix& operator*=(const Matrix&r);

	// Getters

		const float& get(int column, int row)const;
		
		const float& get_value(int column, int row)const;
		float& get_value(int column, int row);

		const float& get_value_b(int row, int col)const;
		float& get_value_b(int row, int col);

		float& operator[](const int index);
		const float& operator[](const int index)const;
	};
}
