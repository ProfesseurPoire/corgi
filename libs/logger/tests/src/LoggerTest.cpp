#include <corgi/logger/log.h>
#include <corgi/test/test.h>

#include <fstream>

using namespace corgi;
using namespace corgi::test;

int main()
{
    corgi::test::run_all();
}

class LoggerTest : public Test
{
    public:

    private:
};

TEST_F(LoggerTest, write_log_in_default_file)
{
    logger::toggle_console_output(false);
    logger::show_time(false);
    log_info("Hi");
    logger::close_files();

    std::ifstream file("logs/default.log");
    assert_that(file.is_open(), equals(true));

    std::string line;
    std::getline(file, line);
    std::string message("Info : {default} : \"Hi\" at (LoggerTest.cpp::run 25) ");
    assert_that(line, equals(message));
}

TEST_F(LoggerTest, write_log_in_another_folder)
{
    logger::set_folder("logs/l");
    logger::show_time(false);
    log_info(10);
    logger::close_files();

    std::ifstream file("logs/l/default.log");
    assert_that(file.is_open(), equals(true));

    std::string line;
    std::getline(file, line);
    std::string message("Info : {default} : \"10\" at (LoggerTest.cpp::run 41) ");
    assert_that(line, equals(message));
}