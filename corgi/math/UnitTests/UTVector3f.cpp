//#include <corgi/math/Vector3f.h>
//#include <ostream>
//using namespace corgi::math;
//
//static std::ostream& operator<<(std::ostream& o, const Vector3f& v)
//{
//    return o << "{ x : " << v.x << " ; y = " << v.y << " ; z = " << v.z << " }";
//}
//
//
//#include <corgi/test/test.h>
//
//using namespace std;
//using namespace corgi::test;
//
//class Vector3Testing : public Test
//{
//public:
//
//	Vector3f u;
//	Vector3f right		= Vector3f(1.0f, 0.0f, 0.0f);
//	Vector3f up			= Vector3f(0.0f, 1.0f, 0.0f);
//	Vector3f one		= Vector3f(1.0f, 1.0f, 1.0f);
//	Vector3f backward	= Vector3f(0.0f, 0.0f, 1.0f);
//	Vector3f v			= Vector3f(1.0f, 2.0f, 3.0f);
//};
//
//TEST_F(Vector3Testing, check_comparison)
//{
//	assert_that(v, equals(v));
//	assert_that(v, non_equals(u));
//}
//
//TEST_F(Vector3Testing, check_cross_product)
//{
//	assert_that(right.cross(up), equals(backward));
//}
//
//TEST_F(Vector3Testing, check_lerp)
//{
//	assert_that(Vector3f::lerp(u, one, 0.5f), equals(Vector3f(0.5f, 0.5f, 0.5f)));
//}
//
//TEST_F(Vector3Testing, Length)
//{
//	assert_that(Vector3f(4.0f, 4.0f, 2.0f).length(), equals(6.0f));
//}
//
//TEST_F(Vector3Testing, check_sqrt_length)
//{
//	assert_that(Vector3f(2.0f, 3.0f, 5.0f).sqrt_length(), equals(38.0f));
//}
//
//TEST_F(Vector3Testing, check_substraction)
//{
//	assert_that(one - one, equals(Vector3f(0.0f, 0.0f, 0.0f)));
//}
//
//TEST_F(Vector3Testing, check_unary_substraction)
//{
//	assert_that(-one, equals(Vector3f(-1.0f, -1.0f, -1.0f)));
//}
//
//TEST_F(Vector3Testing, Addition)
//{
//	assert_that(right + up, equals(Vector3f(1.0f, 1.0f, 0.0f)));
//}
//
//TEST_F(Vector3Testing, Mutliplication)
//{
//	assert_that(v*v, equals(Vector3f(1.0f, 4.0f, 9.0f)));
//	assert_that(v*2.0f, equals(Vector3f(2.0f, 4.0f, 6.0f)));
//}
//
//TEST_F(Vector3Testing, division_arithmetic_operator)
//{
//	assert_that(Vector3f(10.0f, 8.0f, 6.0f) / 2.0f, equals(Vector3f(5.0f, 4.0f, 3.0f)));
//}
//
//TEST_F(Vector3Testing, multiplication_assignment_operator)
//{
//	assert_that(Vector3f(2.0f, 3.0f, 4.0f) *= Vector3f(2.0f, 3.0f, 4.0f),
//		equals(Vector3f(4.0f, 9.0f, 16.0f)));
//
//	assert_that(Vector3f(2.0f, 3.0f, 4.0f) *= 2.0f,
//		equals(Vector3f(4.0f, 6.0f, 8.0f)));
//}
//
//TEST_F(Vector3Testing, assition_assignment_operator)
//{
//	assert_that(Vector3f(2.0f, 3.0f, 5.0f) += Vector3f(1.0f, 3.0f, 2.0f),
//		equals(Vector3f(3.0f, 6.0f, 7.0f)));
//}
//
//TEST_F(Vector3Testing, substraction_assignment_operator)
//{
//	assert_that(Vector3f(2.0f, 3.0f, 5.0f) -= Vector3f(2.0f, 3.0f, 5.0f),
//		equals(Vector3f(0.0f, 0.0f, 0.0f)));
//}
//
//TEST_F(Vector3Testing, division_assignment_operator)
//{
//	assert_that(Vector3f(2.0f, 4.0f, 6.0f) /= 2.0f,
//		equals(Vector3f(1.0f, 2.0f, 3.0f)));
//}
