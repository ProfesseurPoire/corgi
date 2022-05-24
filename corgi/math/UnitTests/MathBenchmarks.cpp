#include <corgi/test/test.h>
#include <corgi/math/Vec2.h>
#include <random>

using namespace corgi;

static int generation = 1000000;

static std::random_device rd;
// By default we simply initialize the generator with
// the random_device
static std::mt19937 mt(rd());

static float sum = 0.0f;


TEST(MathBenchmark, test_random_vector_generation_angle)
{
	for (int i = 0; i < generation; i++)
	{
		sum += Vec2::random(2.0f).x;
	}
}

TEST(MathBenchmark, test_random_vector_generation_normalized)
{
	for (int i = 0; i < generation; i++)
	{
		std::uniform_real_distribution ud
		(
			-1.0f,
			1.0f
		);

		Vec2 v(ud(mt), ud(mt));
		v.normalize();
		v *= 2.0f;
		sum += v.x;
	}
}