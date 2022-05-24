#include <corgi/test/test.h>

using namespace corgi::test;

TEST(TestB,Check)
{
    assert_that(true, equals(true));
}

TEST(TestB,CheckEquals)
{
    int val = 10;

    assert_that(true, equals(true));
}

TEST(TestB,CheckNonEquals)
{
    assert_that(10, non_equals(5));
}