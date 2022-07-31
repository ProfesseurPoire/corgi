//#include <corgi/math/Matrix.h>
//#include <corgi/math/Vec3.h>
//#include <corgi/math/Vec4.h>
//#include <corgi/math/MathUtils.h>
//
//#include <ostream>
//
//using namespace corgi;
//
//static std::ostream& operator<<(std::ostream& o, const Vec3& v)noexcept
//{
//    return o << "{ x : " << v.x << " ; y = " << v.y << " ; z = " << v.z << " }";
//}
//
//static std::ostream& operator<<(std::ostream& o, const corgi::Vec4& v)noexcept
//{
//    return o << "{ x : " << v.x << " ; y = " << v.y << " ; z = " << v.z << " ; w = " << v.w << "}";
//}
//
//static std::ostream& operator<<(std::ostream& o, const Matrix& m)noexcept
//{
//    o << m[0] << " " << m[1] << " " << m[2] << " " << m[3] << "\n";
//    o << m[4] << " " << m[5] << " " << m[6] << " " << m[7] << "\n";
//    o << m[8] << " " << m[9] << " " << m[10] << " " << m[11] << "\n";
//    return  o << m[12] << " " << m[13] << " " << m[14] << " " << m[15] << "\n";
//}
//
//#include <corgi/test/test.h>
//
//using namespace std;
//using namespace corgi::test;
//
//TEST(Matrix, Constructor)
//{
//	Matrix m1( 1.0f, 2.0f, 3.0f, 4.0f,
//		        5.0f, 6.0f, 7.0f, 8.0f,
//		        9.0f, 10.0f, 11.0f, 12.0f,
//		        13.0f, 14.0f, 15.0f, 16.0f);
//
//	assert_that(m1[0],  equals(1.0f));
//	assert_that(m1[4],  equals(2.0f));
//	assert_that(m1[8],  equals(3.0f));
//	assert_that(m1[12], equals(4.0f));
//
//	assert_that(m1[1], equals(5.0f));
//	assert_that(m1[5], equals(6.0f));
//	assert_that(m1[9], equals(7.0f));
//	assert_that(m1[13], equals(8.0f));
//
//	assert_that(m1[2],  equals(9.0f));
//	assert_that(m1[6], equals(10.0f));
//	assert_that(m1[10], equals(11.0f));
//	assert_that(m1[14], equals(12.0f));
//
//	assert_that(m1[3], equals(13.0f));
//	assert_that(m1[7], equals(14.0f));
//	assert_that(m1[11], equals(15.0f));
//	assert_that(m1[15], equals(16.0f));
//
//	Matrix m2;
//
//	assert_that(m2[0], equals(1.0f));
//	assert_that(m2[4], equals(0.0f));
//	assert_that(m2[8], equals(0.0f));
//	assert_that(m2[12], equals(0.0f));
//
//	assert_that(m2[1], equals(0.0f));
//	assert_that(m2[5], equals(1.0f));
//	assert_that(m2[9], equals(0.0f));
//	assert_that(m2[13], equals(0.0f));
//
//	assert_that(m2[2], equals(0.0f));
//	assert_that(m2[6], equals(0.0f));
//	assert_that(m2[10], equals(1.0f));
//	assert_that(m2[14], equals(0.0f));
//
//	assert_that(m2[3], equals(0.0f));
//	assert_that(m2[7], equals(0.0f));
//	assert_that(m2[11], equals(0.0f));
//	assert_that(m2[15], equals(1.0f));
//
//	// In Column order
//	float values[16] = 
//    {
//		1.0f, 5.0f, 9.0f, 13.0f,
//		2.0f, 6.0f, 10.0f, 14.0f,
//		3.0f, 7.0f, 11.0f, 15.0f,
//		4.0f, 8.0f, 12.0f, 16.0f 
//    };
//
//	Matrix m3(values);
//
//	assert_that(m3[0], equals(1.0f));
//	assert_that(m3[4], equals(2.0f));
//	assert_that(m3[8], equals(3.0f));
//	assert_that(m3[12], equals(4.0f));
//
//	assert_that(m3[1], equals(5.0f));
//	assert_that(m3[5], equals(6.0f));
//	assert_that(m3[9], equals(7.0f));
//	assert_that(m3[13], equals(8.0f));
//
//	assert_that(m3[2], equals(9.0f));
//	assert_that(m3[6], equals(10.0f));
//	assert_that(m3[10], equals(11.0f));
//	assert_that(m3[14], equals(12.0f));
//
//	assert_that(m3[3], equals(13.0f));
//	assert_that(m3[7], equals(14.0f));
//	assert_that(m3[11], equals(15.0f));
//	assert_that(m3[15], equals(16.0f));
//}
//
//TEST(Matrix, Comparison)
//{
//	Matrix m1;
//	m1[2] = 2.0f;
//
//	Matrix m2;
//	m2[2] = 2.0f;
//
//	Matrix m3;
//	m3[2] = (4.0f);
//
//	assert_that(m1 == m2, equals(true));
//	assert_that(m1 != m3, equals(true));
//}
//
//TEST(Matrix, Multiplication)
//{
//	// Row order
//	Matrix m(1.0f, 2.0f, 3.0f, 4.0f,
//		5.0f, 6.0f, 7.0f, 8.0f,
//		9.0f, 10.0f, 11.0f, 12.0f,
//		13.0f, 14.0f, 15.0f, 16.0f);
//
//	Matrix result(90.0f, 100.0f, 110.0f, 120.0f,
//		202.0f, 228.0f, 254.0f, 280.0f,
//		314.0f, 356.0f, 398.0f, 440.0f,
//		426.0f, 484.0f, 542.0f, 600.0f);
//
//	assert_that(m*m, equals(result));
//	assert_that(m *= m, equals(result));
//}
//
//TEST(Matrix, Affectation)
//{
//	Matrix m;
//	m[0] = 2.0f;
//
//	Matrix m2(m);
//	Matrix m3;
//	m3 = m;
//
//	assert_that(m2, equals(m));
//	assert_that(m3, equals(m));
//}
//
//// It simpler to group the multiplication and the translation
//// since the translation matrix give easy to understand results
//TEST(Matrix, VectorMultiplication)
//{
//	Matrix m = Matrix::translation(10.0f, 2.0f, 3.0f);
//
//	Vec3 v1(0.0f, 0.0f, 0.0f);
//	corgi::Vec4 v2(1.0f, 2.0f, 3.0f, 1.0f);
//
//	assert_that(m*v1, equals(Vec3(10.0f, 2.0f, 3.0f)));
//	//assert_that(m*v2, equals(corgi::Vec4(11.0f, 4.0f, 6.0f, 1.0f)));
//}
//
//TEST(Matrix, MatrixMultiplicationSpeed)
//{
//	Matrix m = Matrix::translation(10.0f, 2.0f, 3.0f);
//
//	Vec3 v(2.0f, 3.0f, 4.0f);
//	for (int i = 0; i < 3000000; ++i)
//	{
//		m = m * m;
//	}
//	//v = m * v;
//}
//
//TEST(Matrix, OrthoMatrix)
//{
//	Matrix m = Matrix::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.0f, 20.0f);
//	assert_that(m*Vec3(5.0f, -5.0f, 5.0f),
//		equals(Vec3(0.5f, -0.5f, 0.5f)));
//}
//
//TEST(Matrix, RotationMatrix)
//{
//	//Matrix rx = Matrix::rotation_x(pi_f / 2.0f);
//	//Matrix ry = Matrix::rotation_y(pi_f / 2.0f);
//	//Matrix rz = Matrix::rotation_z(pi_f / 2.0f);
//	//
//	//assert_that((rx*Vec3(0.0f, 1.0f, 0.0f)).x,
//	//	almost_equals(0.0f, 0.001f));
//	//
//	//assert_that((rx*Vec3(0.0f, 1.0f, 0.0f)).y,
//	//	almost_equals(0.0f, 0.001f));
//	//
//	//assert_that((rx*Vec3(0.0f, 1.0f, 0.0f)).z,
//	//	almost_equals(1.0f, 0.001f));
//	//
//	//assert_that((ry*Vec3(1.0f, 0.0f, 0.0f)).x,
//	//	almost_equals(0.0f, 0.001f));
//	//
//	//assert_that((ry*Vec3(1.0f, 0.0f, 0.0f)).y,
//	//	almost_equals(0.0f, 0.001f));
//	//
//	//assert_that((ry*Vec3(1.0f, 0.0f, 0.0f)).z,
//	//	almost_equals(-1.0f, 0.001f));
//	//
//	//assert_that((rz*Vec3(1.0f, 0.0f, 0.0f)).x,
//	//	almost_equals(0.0f, 0.001f));
//	//
//	//assert_that((rz*Vec3(1.0f, 0.0f, 0.0f)).y,
//	//	almost_equals(1.0f, 0.001f));
//	//
//	//assert_that((rz*Vec3(1.0f, 0.0f, 0.0f)).z,
//	//	almost_equals(0.0f, 0.001f));
//}
//
//TEST(Matrix, InverseMatrix)
//{
//	Matrix m = Matrix::rotation_x(1.0f)*  Matrix::translation(290, 20, 10);
//	/*Matrix m(2, -1, 0, 0,
//			  -1, 2, -1, 0,
//			  0, -1, 2, 0,
//			  0, 0, 0, 0);*/
//	
//	Matrix inverse = m.inverse();
//	Matrix result = m * inverse;
//
//	// Ok it works but with a little epsilon thing
//	assert_that(result*Vec3(1.0f, 0.0f, 3.0f),
//		equals(Vec3(1.0f, 0.0f, 3.0f)));
//}
//
//TEST(Matrix, FrustumMatrix)
//{
//	Matrix m = Matrix::frustum(-10.0f, 10.0f, -10.0f, 10.0f, 3.0f, 100.0f);
//	// Ok so it seems to works, except I have to divide the vector by it's w value
//	// to get back the "real" coordinate. I don't know if this should be automated
//	// or not so I'll leave it like that for now
//
//	//corgi::Vec4 r = m * corgi::Vec4(0.0f, 0.0f, -100.0f, 1.0f);
//	//assert_that(r / r.w, equals(corgi::Vec4(0.0f, 0.0f, 1.0f, 1.0f)));
//
//	//corgi::Vec4 r2 = m * corgi::Vec4(-10.0f, 10.0f, -3.0f, 1.0f);
//	//assert_that(r2 / r2.w, equals(corgi::Vec4(-1.0f, 1.0f, -1.0f, 1.0f)));
//}
//
//TEST(Matrix, ScaleMatrix)
//{
//	//assert_that(Matrix::scale(10.0f, 2.0f, 3.0f)*corgi::Vec4(1.0f, 2.0f, 3.0f,1.0f),
//	//	equals(corgi::Vec4(10.0f, 4.0f, 9.0f,1.0f)));
//}
//
//TEST(Matrix, TransformMatrix)
//{
//	//Matrix transform_matrix = Matrix::transform(Vec3(1.0f, 0.0f, 0.0f),
//	//	Vec3(0.0f, 0.0f, pi_f / 2.0f),
//	//	Vec3(2.0f, 2.0f, 2.0f));
//   //
//	//assert_that((transform_matrix*Vec3(0.0f, 0.0f, 0.0f)).x,
//	//	almost_equals(0.0f, 0.001f));
//	//
//	//assert_that((transform_matrix*Vec3(0.0f, 0.0f, 0.0f)).y,
//	//	almost_equals(2.0f, 0.001f));
//	//
//	//assert_that((transform_matrix*Vec3(0.0f, 0.0f, 0.0f)).z,
//	//	almost_equals(0.0f, 0.001f));
//}
//
//TEST(Matrix, LookAt)
//{
//	//Matrix m = Matrix::look_at(Vec3(2.0f, 1.0f, 3.0f), Vec3(4.0f, 1.0f, 3.0f));
//	//
//	//// -6 because the this 4 need to be converted to -2 since the camera
//	//// looks in the z-
//	//assert_that(m*corgi::Vec4(4.0f, 1.0f, 3.0f, 1.0f),
//	//	equals(corgi::Vec4(0.0f, 0.0f, -6.0f, 1.0f)));
//}
//
//TEST(Matrix, MatrixDeterminant)
//{
//	// The determinant of the identity matrix is equal to 1
//	Matrix m;
//	assert_that(Matrix::determinant(m), equals(1.0f));
//
//	// Be A a matrix and I it's inverse
//	// Determinant(I) = 1/Determinant(A)
//	m = Matrix::translation(1.0f, 2.0f, 3.0f);
//	Matrix i = m.inverse();
//
//	assert_that(Matrix::determinant(i),
//		equals(1.0f / Matrix::determinant(m)));
//}