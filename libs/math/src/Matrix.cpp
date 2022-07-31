#include <corgi/math/Matrix.h>
#include <corgi/math/MathUtils.h>

namespace corgi {

	// @brief  Returns the max value inside a column of the matrix
	/*static float max_value(const Matrix& m, const int column_index, const int last_pivot, int& k)
	{
		float max = 0.0f;

		for (int i = last_pivot +1 ; i < 4; ++i)
		{
			float val = math::absf(m.get_value(column_index, i));
			if (val > max)
			{
				k = i;
				max = val;
			}
		}
		return max;
	}*/

	// @brief  Swap the lines from the matrix
	// swap rows
	/*static void swap_lines(Matrix& m, Matrix& identity, const int line_a, const int line_b) 
	{
		for (int i = 0; i < 4; ++i)
		{
			float temp = m.get_value(i, line_a);
			m.get_value(i, line_a) = m.get_value(i, line_b);
			m.get_value(i, line_b) = temp;

			temp = identity.get_value(i, line_a);
			identity.get_value(i, line_a) = identity.get_value(i, line_b);
			identity.get_value(i, line_b) = temp;
		}
	}*/

	//static void substract_rows(Matrix& m, Matrix& identity, int column, int row_pivot, int k) 
	//{
	//	// Warning, I'm not entirely sure if I have to use k or row_pivot in the set_value thing
	//	// for each row
	//	for (int i = 0; i < 4; ++i)
	//	{
	//		// We don't substract the row where the pivot was found
	//		if (i != row_pivot)
	//		{
	//			const float val = m.get_value(column, i);

	//			// for each column
	//			for (int j = 0; j < 4; ++j)
	//			{
	//				m.get_value(j, i)			-=	(m.get_value(j, k) * val);
	//				identity.get_value(j, i)	-=	(identity.get_value(j, k) * val);
	//			}
	//		}
	//	}
	//}

	/*static void divide_row(Matrix& matrix, Matrix& inverse, const int row_pivot, const float value)
	{
		for (int i = 0; i < 4; ++i)
		{
			matrix.get_value(i, row_pivot)	/= value;
			inverse.get_value(i, row_pivot) /= value;
		}
	}*/

	float Matrix::determinant(const Matrix& m) 
	{
		return((m[0] * determinant(m[5], m[9], m[13], m[6], m[10],  m[14], m[7], m[11], m[15])) -
			(   m[4] * determinant(m[1], m[9], m[13], m[2], m[10],  m[14], m[3], m[11], m[15])) +
			(   m[8] * determinant(m[1], m[5], m[13], m[2], m[6],   m[14], m[3], m[7],  m[11])) -
			(   m[12]* determinant(m[1], m[5], m[9] , m[2], m[6],   m[10], m[3], m[7],  m[11])));
	}

	float Matrix::determinant(const float a, const float b, const float c, const float d, const float e, const float f, const float g, const float h, const float i) 
	{
		return ((a * determinant(e, f, h, i)) - (b * determinant(d, f, g, i)) + (c * determinant(d, e, g, h)));
	}

	float Matrix::determinant(const float a, const float b, const float c, const float d)
	{
		return ((a * d) - (b * c));
	}

	Matrix Matrix::look_at(const Vec3 & eye, const Vec3 & center)
	{
		// We get the vector between to and from
		Vec3 forward = (center - eye);
		forward.normalize();

		//  We compute the cross product to find the rotation axis
		Vec3 side = Vec3(0.0f, 0.0, 0.0f).cross(forward);
		side.normalize();

		Vec3  up = forward.cross(side);
		up.normalize();

		return Matrix
		(
			side.x, side.y, side.z, -side.dot(eye),
			up.x, up.y, up.z, -up.dot(eye),
			-forward.x, -forward.y, -forward.z, -forward.dot(eye),
			0.0f, 0.0f, 0.0f, 1.0f
		);
	}

	Matrix Matrix::transform(const Vec3 & t, const Vec3 & r, const Vec3 & s) 
	{
		return scale(s.x, s.y, s.z)*
			euler_angles(r.x, r.y, r.z)*
			translation(t.x, t.y, t.z);
	}

	
	Matrix Matrix::euler_angles(const float x, const float y, const float z)
	{
		return rotation_x(x)*rotation_y(y)*rotation_z(z);
	}
	
	Matrix Matrix::translation(const float x, const float y, const float z)
	{
		return Matrix(
			1.0f, 0.0f, 0.0f, x,
			0.0f, 1.0f, 0.0f, y,
			0.0f, 0.0f, 1.0f, z,
			0.0f, 0.0f, 0.0f, 1.0f);
	}

	Matrix Matrix::translation(const Vec3 & t)
	{
		return translation(t.x, t.y, t.z);
	}

	
	Matrix Matrix::ortho(const float l, const float r, const float b, const float t, const float zn, const float zf) 
	{
		const float rml = r - l;
		const float tmb = t - b;
		const float fmn = zf - zn;

		// I to removed the minus to the equation for it to work on opengl on
		// (zf+zn)/fmn) since the z is inverted in OpenGL (we look in Z-)
		return Matrix
		(
			2.0f / rml,	0.0f,		0.0f,			-(r + l) / (rml),
			0.0f,		2.0f/ tmb,	0.0f,			-(t + b) / (tmb),
			0.0f,		0.0f,		-2.0f / fmn,	-(zf + zn) / (fmn),
			0.0f,		0.0f,		0.0f,			1.0f
		);
	}
	
	Matrix Matrix::frustum(const float left, const float right, const float bottom, const float top, const float znear, const float zfar)
	{
		const float rml = right - left;
		const float tmb = top - bottom;
		const float fmn = zfar - znear;

		const float zero = 0.0f;
		const float two = 2.0f;
		const float one = 1.0f;

		const float ttn = two * znear; //ttn = two times near

		return Matrix
		(
			(ttn) / (rml), zero, (right + left) / (rml), 0.0,
			zero, (ttn) / (tmb), (top + bottom) / (tmb), 0.0,
			zero, zero, -((zfar + znear) / (fmn)), -(ttn*zfar) / (fmn),
			zero, zero, -one, 0.0
		);
	}
	
	Matrix Matrix::rotation_x(const float angle) 
	{
		const float cosa = math::cos(angle);
		const float sina = math::sin(angle);

		return Matrix
		(
			1.0f, 0.0f,	0.0f,	0.0f,
			0.0f, cosa,	-sina,	0.0f,
			0.0f, sina,	cosa,	0.0f,
			0.0f, 0.0f,	0.0f,	1.0f
		);
	}
	
	Matrix Matrix::rotation_y(const float radian)
	{
		const float cosa = math::cos(radian);
		const float sina = math::sin(radian);

		return Matrix
		(
			cosa,  0.0f, sina, 0.0f,
			0.0f,  1.0f, 0.0f, 0.0f,
			-sina, 0.0f, cosa, 0.0f,
			0.0f,  0.0f, 0.0f, 1.0f
		);
	}

	
	Matrix Matrix::rotation_z(const float angle)
	{
		Matrix m;

		const float cosa = math::cos(angle);
		const float sina = math::sin(angle);

        m[0] = cosa;
        m[4] = -sina;

        m[1] = sina;
        m[5] = cosa;

		return m;
	}

	
	Matrix Matrix::scale(const float x, const float y, const float z)
	{
		Matrix m;

        m[0]    = x;
        m[5]    = y;
        m[10]   = z;

		return m;
	}

	// Constructors 

		
	Matrix::Matrix() 
		:
		_items{
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
	} {}

	 Matrix::Matrix(float array[16]) 
		:
		_items{
				array[0],	array[1],	array[2],	array[3],
				array[4],	array[5],	array[6],	array[7],
				array[8],	array[9],	array[10],	array[11],
				array[12],	array[13],	array[14],	array[15]
	} {}

	 Matrix::Matrix(	float m11, float m12, float m13, float m14,
						float m21, float m22, float m23, float m24,
						float m31, float m32, float m33, float m34,
						float m41, float m42, float m43, float m44) :
		_items{
				m11, m21, m31, m41,
				m12, m22, m32, m42,
				m13, m23, m33, m43,
				m14, m24, m34, m44
	} {}

	 Matrix Matrix::operator/(const float v)const
	 {
		 Matrix m = *this;

		 for (int i = 0; i < 16; ++i)
		 {
			 m._items[i] /= v;
		 }
		 return m;
	 }

	 Matrix Matrix::operator*(const float v)const
	 {
		 Matrix m = *this;

		 for (int i = 0; i < 16; ++i)
		 {
			 m._items[i] *= v;
		 }
		 return m;
	 }

	Matrix Matrix::inverse()const
	{
		// This is GLM way to compute the inverse matrix

			const auto Coef00 = get(2,2) * get(3,3) - get(3,2) * get(2,3);
			const auto Coef04 = get(2,1) * get(3,3) - get(3,1) * get(2,3);
			const auto Coef08 = get(2,1) * get(3,2) - get(3,1) * get(2,2);
			const auto Coef12 = get(2,0) * get(3,3) - get(3,0) * get(2,3);
			const auto Coef16 = get(2,0) * get(3,2) - get(3,0) * get(2,2);
			const auto Coef20 = get(2,0) * get(3,1) - get(3,0) * get(2,1);

			const Vec4 Fac0(Coef00, Coef00, get(1,2) * get(3,3) - get(3,2) * get(1,3), get(1,2) * get(2,3) - get(2,2) * get(1,3));
			const Vec4 Fac1(Coef04, Coef04, get(1,1) * get(3,3) - get(3,1) * get(1,3), get(1,1) * get(2,3) - get(2,1) * get(1,3));
			const Vec4 Fac2(Coef08, Coef08, get(1,1) * get(3,2) - get(3,1) * get(1,2), get(1,1) * get(2,2) - get(2,1) * get(1,2));
			const Vec4 Fac3(Coef12, Coef12, get(1,0) * get(3,3) - get(3,0) * get(1,3), get(1,0) * get(2,3) - get(2,0) * get(1,3));
			const Vec4 Fac4(Coef16, Coef16, get(1,0) * get(3,2) - get(3,0) * get(1,2), get(1,0) * get(2,2) - get(2,0) * get(1,2));
			const Vec4 Fac5(Coef20, Coef20, get(1,0) * get(3,1) - get(3,0) * get(1,1), get(1,0) * get(2,1) - get(2,0) * get(1,1));

			const Vec4 Vec0(get_value(1,0), get_value(0,0), get_value(0,0), get_value(0,0));
			const Vec4 Vec1(get_value(1,1), get_value(0,1), get_value(0,1), get_value(0,1));
			const Vec4 Vec2(get_value(1,2), get_value(0,2), get_value(0,2), get_value(0,2));
			const Vec4 Vec3(get_value(1,3), get_value(0,3), get_value(0,3), get_value(0,3));

			const Vec4 Inv0(Vec1 * Fac0 - Vec2 * Fac1 + Vec3 * Fac2);
			const Vec4 Inv1(Vec0 * Fac0 - Vec2 * Fac3 + Vec3 * Fac4);
			const Vec4 Inv2(Vec0 * Fac1 - Vec1 * Fac3 + Vec3 * Fac5);
			const Vec4 Inv3(Vec0 * Fac2 - Vec1 * Fac4 + Vec2 * Fac5);

			const Vec4 SignA(+1, -1, +1, -1);
			const Vec4 SignB(-1, +1, -1, +1);

			const Matrix Inverse(
				(Inv0 * SignA).x, (Inv1 * SignB).x, (Inv2 * SignA).x, (Inv3 * SignB).x,
				(Inv0 * SignA).y, (Inv1 * SignB).y, (Inv2 * SignA).y, (Inv3 * SignB).y,
				(Inv0 * SignA).z, (Inv1 * SignB).z, (Inv2 * SignA).z, (Inv3 * SignB).z,
				(Inv0 * SignA).w, (Inv1 * SignB).w, (Inv2 * SignA).w, (Inv3 * SignB).w);

			const Vec4 Dot0(	get_value(0, 0) * Inverse.get_value(0,0),
						  get_value(0, 1) * Inverse.get_value(1,0),
						  get_value(0, 2) * Inverse.get_value(2,0),
						  get_value(0, 3) * Inverse.get_value(3,0)
						  );

			return Inverse * (1.0f / (Dot0.x + Dot0.y) + (Dot0.z + Dot0.w));



		// Use the Gauss - Jordan Method, algorithm explanation here :
		// https://fr.wikipedia.org/wiki/%C3%89limination_de_Gauss-Jordan

		// This algorithm will transform the current matrix to an
		// identity matrix. By registering the operations the makes the current matrix
		// an identity matrix inside an identity matrix, we actually get a matrix that 
		// register the operations needed to inverse it.

		// I need to find a way to only work on the identity_matrix, maybe?

		//Matrix current_matrix = *this;
		//// The identity matrix is going to be the inverse matrix
		//Matrix identity_matrix;

		//// r is the index of the last row where the pivot was found
		//int r = -1;

		//// For each column
		//for (int i = 0; i < 4; i++)
		//{
		//	// So K is the index of the row with the maximum value
		//	int k		= 0;
		//	float m		= max_value(current_matrix, i, r, k);

		//	if (current_matrix.get_value(i, k) !=0)
		//	{
		//		r += 1;
		//		// I think theses 2 are safe
		//		divide_row(current_matrix, identity_matrix, k, m);
		//		swap_lines(current_matrix, identity_matrix, r, k);

		//		substract_rows(current_matrix, identity_matrix, i, r, k);
		//	}
		//}
		//return identity_matrix;
	}

	void Matrix::identity() 
	{
		_items[0] = 1.0f; _items[4] = 0.0f; _items[8]  = 0.0f;  _items[12] = 0.0f;
		_items[1] = 0.0f; _items[5] = 1.0f; _items[9]  = 0.0f;  _items[13] = 0.0f;
		_items[2] = 0.0f; _items[6] = 0.0f; _items[10] = 1.0f;  _items[14] = 0.0f;
		_items[3] = 0.0f; _items[7] = 0.0f; _items[11] = 0.0f;  _items[15] = 1.0f;
	}

	// Arithmetics operators
		
		Matrix Matrix::operator*(const Matrix& r)const
		{
			// column order
			return Matrix
			(
				_items[0] * r._items[0] + _items[4] * r._items[1] + _items[8] * r._items[2]  + _items[12] * r._items[3],
				_items[0] * r._items[4] + _items[4] * r._items[5] + _items[8] * r._items[6]  + _items[12] * r._items[7],
				_items[0] * r._items[8] + _items[4] * r._items[9] + _items[8] * r._items[10] + _items[12] * r._items[11],
				_items[0] * r._items[12]+ _items[4] * r._items[13]+ _items[8] * r._items[14] + _items[12] * r._items[15],

				_items[1] * r._items[0] + _items[5] * r._items[1] + _items[9] * r._items[2]  + _items[13] * r._items[3],
				_items[1] * r._items[4] + _items[5] * r._items[5] + _items[9] * r._items[6]  + _items[13] * r._items[7],
				_items[1] * r._items[8] + _items[5] * r._items[9] + _items[9] * r._items[10] + _items[13] * r._items[11],
				_items[1] * r._items[12]+ _items[5] * r._items[13]+ _items[9] * r._items[14] + _items[13] * r._items[15],

				_items[2] * r._items[0] + _items[6] * r._items[1] + _items[10] * r._items[2] + _items[14] * r._items[3],
				_items[2] * r._items[4] + _items[6] * r._items[5] + _items[10] * r._items[6] + _items[14] * r._items[7],
				_items[2] * r._items[8] + _items[6] * r._items[9] + _items[10] * r._items[10]+ _items[14] * r._items[11],
				_items[2] * r._items[12]+ _items[6] * r._items[13]+ _items[10] * r._items[14]+ _items[14] * r._items[15],

				_items[3] * r._items[0] + _items[7] * r._items[1] + _items[11] * r._items[2]  +_items[15] * r._items[3],
				_items[3] * r._items[4] + _items[7] * r._items[5] + _items[11] * r._items[6]  +_items[15] * r._items[7],
				_items[3] * r._items[8] + _items[7] * r._items[9] + _items[11] * r._items[10] +_items[15] * r._items[11],
				_items[3] * r._items[12]+ _items[7] * r._items[13]+ _items[11] * r._items[14] +_items[15] * r._items[15]
			);
		}

	
	Vec3 Matrix::operator*(const Vec3 & v)const
	{
		return Vec3
		(
			_items[0] * v.x + _items[4] * v.y + _items[8]  * v.z + _items[12],
			_items[1] * v.x + _items[5] * v.y + _items[9]  * v.z + _items[13],
			_items[2] * v.x + _items[6] * v.y + _items[10] * v.z + _items[14]
		);
	}

	Vec2 Matrix::operator*(const Vec2& v) const
	{
		return Vec2
		(
			_items[0] * v.x + _items[4] * v.y + _items[12],
			_items[1] * v.x + _items[5] * v.y + _items[13]
		);
	}

	Vec4 Matrix::operator*(const Vec4& v)const
	{
		return Vec4
		(
			_items[0] * v.x + _items[4] * v.y+ _items[8] *  v.z + _items[12] * v.w,
			_items[1] * v.x + _items[5] * v.y+ _items[9] *  v.z + _items[13] * v.w,
			_items[2] * v.x + _items[6] * v.y+ _items[10] * v.z + _items[14] * v.w,
			_items[3] * v.x + _items[7] * v.y+ _items[11] * v.z + _items[15] * v.w
		);
	}

// Comparison operators

	bool Matrix::operator == (const Matrix& m)const
	{
		for (int i = 0; i < 16; ++i)
		{
			if (m._items[i] != _items[i])
			{
				return false;
			}
		}
		return true;
	}

	bool Matrix::operator!=(const Matrix& m)const
	{
		for (int i = 0; i < 16; ++i)
		{
			if (m._items[i] != _items[i])
			{
				return true;
			}
		}
		return false;
	}

// Assignement operators

	Matrix& Matrix::operator*=(const Matrix&r)
	{
		return *this = *this*r;
	}

	const float& Matrix::get(int column, int row)const
	{
		return _items[column * 4 + row];
	}

	// Getters

	const float& Matrix::get_value_b(int row, int col)const
	{
		return _items[row * 4 + col];
	}

	float& Matrix::get_value_b(int row, int col)
	{
		return _items[row * 4 + col];
	}

	const float& Matrix::get_value(int column, int row)const
	{
		return _items[column * 4 + row];
	}

	float& Matrix::get_value(int column, int row)
	{
		return _items[column * 4 + row];
	}

    float& Matrix::operator[](const int index)
    {
        return _items[index];
    }

    const float& Matrix::operator[](const int index)const
    {
        return _items[index];
    }
}