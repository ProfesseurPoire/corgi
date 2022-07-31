//#include <corgi/math/Vec4.h>
//#include <corgi/math/MathUtils.h>
//
//#include <ostream>
//using namespace corgi::math;
//
//static std::ostream& operator<<(std::ostream& o, const corgi::Vec4& v)
//{
//    return o << "{ x : " << v.x << " ; y = " << v.y << " ; z = " << v.z << " ; w = " << v.w << "}";
//}
//
//#include <corgi/test/test.h>
//
//using namespace std;
//using namespace corgi::test;
//
//
//class Vector4Testing : public Test
//{
//public:
//	
//	corgi::Vec4 one		= corgi::Vec4(1.0f, 1.0f, 1.0f, 1.0f);
//	corgi::Vec4 zero		= corgi::Vec4(0.0f, 0.0f, 0.0f, 0.0f);
//	corgi::Vec4 right		= corgi::Vec4(1.0f, 0.0f, 0.0f, 0.0f);
//	corgi::Vec4 up			= corgi::Vec4(0.0f, 1.0f, 0.0f, 1.0f);
//	corgi::Vec4 down		= corgi::Vec4(0.0f, -1.0f, 0.0f, 1.0f);
//	corgi::Vec4 forward	= corgi::Vec4(0.0f, 0.0f, 1.0f, 1.0f);
//};
//
//TEST_F(Vector4Testing, Constructors)
//{
//	corgi::Vec4 u;
//	assert_that(u.x, equals(0.0f));
//	assert_that(u.y, equals(0.0f));
//	assert_that(u.z, equals(0.0f));
//	assert_that(u.w, equals(0.0f));
//
//	corgi::Vec4 v(1.0f, 2.0f, 3.0f, 2.0f);
//	assert_that(v.x, equals(1.0f));
//	assert_that(v.y, equals(2.0f));
//	assert_that(v.z, equals(3.0f));
//	assert_that(v.w, equals(2.0f));
//}
//
//TEST_F(Vector4Testing, check_comparison)
//{
//	assert_that(zero, equals(zero));
//	assert_that(zero, non_equals(one));
//}
////
////TEST_F(Vector4Testing, check_cross_product)
////{
////	assert_that(right.cross(up), equals(forward));
////}
////
////TEST_F(Vector4Testing, check_lerp)
////{
////	assert_that(corgi::Vec4::lerp(zero, corgi::Vec4(10.0f, 10.0f, 10.0f, 10.0f), 0.5f),
////		equals(corgi::Vec4(5.0f, 5.0f, 5.0, 5.0f)));
////}
////
////TEST_F(Vector4Testing, check_length)
////{
////	assert_that(corgi::Vec4(4.0f, 4.0f, 2.0f, 0.0f).length(), equals(6.0f));
////}
////
////TEST_F(Vector4Testing, check_sqrt_length)
////{
////	assert_that(corgi::Vec4(2.0f, 3.0f, 5.0f, 0.0f).sqrt_length(), equals(38.0f));
////}
////
////TEST_F(Vector4Testing, check_substraction)
////{
////	assert_that(one - one, equals(zero));
////}
////
////TEST_F(Vector4Testing, check_unary_substraction)
////{
////	assert_that(-one, equals(corgi::Vec4(-1.0f, -1.0f, -1.0f, -1.0f)));
////}
////
////TEST_F(Vector4Testing, check_addition)
////{
////	assert_that(one + one, equals(corgi::Vec4(2.0f, 2.0f, 2.0f, 2.0f)));
////}
////
////TEST_F(Vector4Testing, check_multiplication)
////{
////	assert_that(corgi::Vec4(2.0f, 3.0f, 4.0f, 1.0f)*corgi::Vec4(2.0f, 3.0f, 4.0f, 1.0f),
////		equals(corgi::Vec4(4.0f, 9.0f, 16.0f, 1.0f)));
////
////	assert_that(corgi::Vec4(2.0f, 3.0f, 4.0f, 1.0f)*2.0f,
////		equals(corgi::Vec4(4.0f, 6.0f, 8.0f, 2.0f)));
////}
////
////TEST_F(Vector4Testing, Division)
////{
////	assert_that(corgi::Vec4(10.0f, 8.0f, 6.0f, 0.0f) / 2.0f,
////		equals(corgi::Vec4(5.0f, 4.0f, 3.0f, 0.0f)));
////}
////
////TEST_F(Vector4Testing, AssignmentMutliplication)
////{
////	assert_that(corgi::Vec4(2.0f, 3.0f, 4.0f, 5.0f) *= corgi::Vec4(2.0f, 3.0f, 4.0f, 5.0f),
////		equals(corgi::Vec4(4.0f, 9.0f, 16.0f, 25.0f)));
////
////	assert_that(corgi::Vec4(2.0f, 3.0f, 4.0f, 1.0f) *= 2.0f,
////		equals(corgi::Vec4(4.0f, 6.0f, 8.0f, 2.0f)));
////}
////
////TEST_F(Vector4Testing, check_assignment_addition)
////{
////	assert_that(corgi::Vec4(2.0f, 3.0f, 5.0f, 1.0f) += corgi::Vec4(1.0f, 3.0f, 2.0f, 1.0f),
////		equals(corgi::Vec4(3.0f, 6.0f, 7.0f, 2.0f)));
////}
////
////TEST_F(Vector4Testing, check_assignment_substraction)
////{
////	assert_that(corgi::Vec4(2.0f, 3.0f, 5.0f, 1.0f) -= corgi::Vec4(2.0f, 3.0f, 5.0f, 1.0f),
////		equals(corgi::Vec4(0.0f, 0.0f, 0.0f, 0.0f)));
////}
////
////TEST_F(Vector4Testing, check_assignment_division)
////{
////	assert_that(corgi::Vec4(2.0f, 4.0f, 6.0f, 2.0f) /= 2.0f,
////		equals(corgi::Vec4(1.0f, 2.0f, 3.0f, 1.0f)));
////}
//
