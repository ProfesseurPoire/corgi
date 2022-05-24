#include <corgi/test/test.h>

using namespace corgi::test;

class TestFixture : public corgi::test::Test
{
public:

    void set_up() override
    {
        x = 45;
        y = 10;
    }

    void tear_down() override
    {
        x = 0;
    }

    int x = 0;
    int y = 0;
};

TEST_F(TestFixture, use_assert_that)
{
    std::string str("a");

    assert_that("a", equals(str));
    assert_that(y, equals(10));
    assert_that(y, non_equals(2));
    assert_that(0.1f, almost_equals(0.09f, 0.0200f));
}