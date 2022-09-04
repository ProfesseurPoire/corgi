#include "FontLoader.h"
#include "config.h"

#include <corgi/test/test.h>

using namespace corgi::test;

class TestFontLoader : public Test
{
protected:
    std::string font_file_;
    std::string output_file_;

    std::string font_file_2_;
    std::string output_file_2_;

public:
    void set_up() override
    {
        font_file_ = RESOURCE_DIRECTORY;
        font_file_ += "/Roboto-Black.ttf";
        output_file_ = RESOURCE_DIRECTORY;
        output_file_ += "/Roboto-Black.fnt";

        font_file_2_ = RESOURCE_DIRECTORY;
        font_file_2_ += "/comic_sans.ttf";
        output_file_2_ = RESOURCE_DIRECTORY;
        output_file_2_ += "/comic_sans.fnt";
    }

    void tear_down() override {}
};

TEST_F(TestFontLoader, initialization)
{
    assert_that(FontLoader::initialize(), equals(true));
}

TEST_F(TestFontLoader, try_load)
{
    assert_that(FontLoader::load(font_file_.c_str(), output_file_.c_str()), equals(true));
    assert_that(FontLoader::load(font_file_2_.c_str(), output_file_2_.c_str()),
                equals(true));

    FontLoader::finalize();
}

int main()
{
    run_all();
}