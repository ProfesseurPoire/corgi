#include <corgi/math/Vec2.h>
#include <corgi/math/MathUtils.h>

#include <ostream>
using namespace corgi::math;
//
//static std::ostream& operator<<(std::ostream& o, const Vector2f& v)
//{
//    return o << "{ x : " << v.x << " ; y = " << v.y << " }";
//}
//
//#include <corgi/test/test.h>
//
//using namespace std;
//using namespace corgi::test;
//
//
//
//class Vector2Test : public corgi::test::Test {};
//
//// TODO : make an executable to test for speed, apart 
//// from the unit test. Also maybe add Clock directly to the
//// testing framework
//
////
////class Clock
////{
////public:
////
////	void start()
////	{
////		_start = std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
////	}
////
////	double elapsed_time()
////	{
////		return std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count() - _start;
////	}
////
////private:
////
////	double _start = std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();;
////};
//
////void assignStuff(const std::array<float, 150000> & start, std::array<float, 150000> & destination, int s, int c)
////{
////	for (int i = 0; i < c; ++c)
////	{
////		destination[s + i] = start[s + i];
////	}
////}
//
////
////TEST_F(Vector2Test, speed)
////{
////	//std::vector<float> basev;
////	//std::array<float, 150000> base;
////	//std::array<float, 150000> output;
////
////	//for (int i = 0; i < base.size(); ++i)
////	//{
////	//	basev.push_back((std::rand() % 200) + 1);
////	//	base[i] = (std::rand() % 200) + 1;
////	//}
////
////	//Clock c;
////	//c.start();
////	//output = base;
////	//std::cout << c.elapsed_time() << std::endl;
////
////	//c.start();
////
////	//for (int i = 0; i < base.size(); ++i)
////	//{
////	//	output[i] = base[i];
////	//}
////	//std::cout << c.elapsed_time() << std::endl;
////
////	//c.start();
////	//memcpy(output.data(), base.data(), sizeof(float)*base.size());
////	//std::cout << c.elapsed_time() << std::endl;
////
////	//c.start();
////	//std::copy_n(basev.begin(), basev.size(), output.begin());
////	//std::cout << c.elapsed_time() << std::endl;
////
////	//c.start();
////	//memcpy(output.data(), basev.data(), sizeof(float)*base.size());
////	//std::cout << c.elapsed_time() << std::endl;
////
////	//c.start();
////	//std::copy(basev.begin(), basev.end(), output.begin());
////	//std::cout << c.elapsed_time() << std::endl;
////
////	//c.start();
////	//std::move(basev.begin(), basev.end(), output.begin());
////	//std::cout << c.elapsed_time() << std::endl;
////
////	//c.start();
////	//std::thread p1(assignStuff, base, output, 0, 75000);
////	//std::thread p2(assignStuff, base, output, 75000, 75000);
////
////	//p1.join();
////	//p2.join();
////	//std::cout << c.elapsed_time() << std::endl;
////
////}
//
//TEST_F(Vector2Test, check_equals)
//{
//	assert_that(Vector2f(1.0f, 0.0f), equals(Vector2f(1.0f, 0.0f)));
//}
//
//TEST_F(Vector2Test, check_not_equals)
//{
//	assert_that(Vector2f(1.0f, 0.0f), non_equals(Vector2f(0.0f, 1.0f)));
//}
//
//TEST_F(Vector2Test, check_default_constructor)
//{
//	Vector2f d;
//
//	assert_that(d.x, equals(0.0f));
//	assert_that(d.y, equals(0.0f));
//
//	Vector2f dd(0.0f, 1.0f);
//
//	assert_that(dd.x, equals(0.0f));
//	assert_that(dd.y, equals(1.0f));
//}
//
//TEST_F(Vector2Test, check_lerp)
//{
//	assert_that(Vector2f::lerp(Vector2f(0.0f, 0.0f),
//		Vector2f(0.0f, 1.0f), 0.5f),
//		equals(Vector2f(0.0f, 0.5f)));
//}
//
//TEST_F(Vector2Test, check_addition)
//{
//	assert_that(Vector2f(2.0f, 4.0f) + Vector2f(2.0f, 6.0f),
//		equals(Vector2f(4.0f, 10.0f)));
//}
//
//TEST_F(Vector2Test, check_subtraction)
//{
//	assert_that(Vector2f(6.0f, 4.0f) - Vector2f(4.0f, 0.0f),
//		equals(Vector2f(2.0f, 4.0f)));
//}
//
//TEST_F(Vector2Test, check_multiplication)
//{
//	assert_that(Vector2f(2.0f, 5.0f)*Vector2f(2.0f, 5.0f),
//		equals(Vector2f(4.0f, 25.0f)));
//}
//
//TEST_F(Vector2Test, check_orthogonal_vector)
//{
//	assert_that(Vector2f(0.0f, 1.0f).orthogonalVector(),
//		equals(Vector2f(1.0f, 0.0f)));
//}
//
//TEST_F(Vector2Test, check_ortho)
//{
//	assert_that(Vector2f(0.0f, 1.0f).dot(Vector2f(1.0f, 0.0f)),
//		equals(0.0f));
//
//	assert_that(Vector2f::areOrtho(Vector2f(0.0f, 1.0f),Vector2f(1.0f, 0.0f)),
//		equals(true));
//}
//
//TEST_F(Vector2Test, check_colinearity)
//{
//	assert_that(Vector2f(0.0f, 1.0f).areColinear(Vector2f(0.0f, 1.0f)),
//		equals(true));
//}
//
//TEST_F(Vector2Test, check_angle)
//{
//	assert_that(Vector2f(0.0f, 1.0f).angle(Vector2f(1.0f, 0.0f)),
//		equals(pi_f / 2.0f));
//}
//
//TEST_F(Vector2Test, CheckLength)
//{
//	Vector2f v = Vector2f(2.0f, 0.0f);
//	assert_that(v.length(), equals(2.0f));
//}
//
//TEST_F(Vector2Test, check_normalize)
//{
//	Vector2f v = Vector2f(2.0f, 0.0f);
//	v = v.normalized();
//	//assert_that(v.normalize(), equals(true));
//	assert_that(v, equals(Vector2f(1.0f, 0.0f)));
//}
