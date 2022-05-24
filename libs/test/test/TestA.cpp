#include <corgi/test/test.h>

using namespace corgi::test;

TEST(TestA,Check)
{
    assert_that(true, corgi::test::equals(true));
}

TEST(TestA,CheckEquals)
{
    int val = 10;

    assert_that(true, corgi::test::equals(true));
}

TEST(TestA,CheckNonEquals)
{
    assert_that(10, corgi::test::non_equals(5));
}