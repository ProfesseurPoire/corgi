#include <corgi/test/test.h>
#include <corgi/CString.h>
#include <corgi/SimpleString.h>

using namespace corgi::test;
using namespace corgi;

static int sample = 5000;
static const char* sample_world = "Hello sdfgsdgfsdgsgsddsgsdfgsdg";

static int j=0;

class StringBenchmark : public Test
{

public:
    // We're generating random strings just to make sure the benchmark results
    // aren't affected by branch prediction and whatnot. Because I noticed that
    // the second test was often faster when working with the same dataset
    std::string random_string(size_t length)
    {
        auto randchar = []() -> char
        {
            const char charset[] =
                "0123456789"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "abcdefghijklmnopqrstuvwxyz";
            const size_t max_index = (sizeof(charset) - 1);
            return charset[rand() % max_index];
        };
        std::string str(length, 0);
        std::generate_n(str.begin(), length, randchar);
        return str;
    }

    void set_up() override
    {
        for(int i=0; i< sample; i++)
        {
            _std_strings[i] = random_string(30 + rand() % 2);
            _strings[i] = _std_strings[i].c_str();
        }
    }

    std::vector<const char*> _strings       = std::vector<const char*>(sample);
    std::vector<std::string> _std_strings   = std::vector<std::string>(sample);
};


TEST_F(StringBenchmark, CStringConstruction)
{
    for (int i = 0; i < sample; i++)
    {
        SimpleString str(_strings[i]);
    }
}

TEST_F(StringBenchmark, STLStringConstruction)
{
    for (int i = 0; i < sample; i++)
    {
        std::string str(_strings[i]);
    }
}



//
//TEST(CStringTesting , size)
//{
//    assert_that(CString::size("corgi"), equals(8));
//}
//
//TEST(CStringTesting, copy)
//{
//    char out[9];
//
//    CString::copy("corgi", out);
//
//    assert_that(CString::size(out), equals(8));
//
//    assert_that(out[0], equals('C'));
//    assert_that(out[1], equals('a'));
//    assert_that(out[2], equals('b'));
//    assert_that(out[3], equals('b'));
//    assert_that(out[4], equals('a'));
//    assert_that(out[5], equals('g'));
//    assert_that(out[6], equals('e'));
//    assert_that(out[7], equals('s'));
//    assert_that(out[8], equals('\0'));
//}
//
//TEST(CStringTest, compare)
//{
//    assert_that(CString::compare("corgi", "corgi"), equals(true));
//    assert_that(CString::compare("corgi", "Notcorgi"), equals(false));
//}
//
//TEST(CStringTest, text_match)
//{
//    assert_that(CString::text_match("corgi", "corgi"),       equals(true));
//    assert_that(CString::text_match("stuff", "corgi"),       equals(false));
//    assert_that(CString::text_match("corgi", "corgi"),       equals(false));
//    assert_that(CString::text_match("corgi", "corgi", false), equals(true));
//}