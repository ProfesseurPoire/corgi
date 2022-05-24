#pragma once

#include <vector>
#include <iostream>
#include <map>
#include <string>
#include <ctime>
#include <chrono>
#include <functional>
#include <memory>
#include <sstream>

/*!
 * @brief      Provides a framework to make test driven development easier
 * @details    Use the TEST macro to define your testing functions. 
 * The framework use this macro to register your tests inside the 
 * @ref _test_functions variable.
 *    
 * After defining a test function through the TEST macro,use the CHECK, CHECK_EQ, CHECK_NE
 * on conditions that can fail. If a check fails, an error will be automatically
 * notified and logged in the console through std::cerr
 *
 * To launch all the register test functions, use the @ref start function
*/
namespace corgi { namespace test {

using String = std::string;
template<class T> using UniquePtr = std::unique_ptr<T>;

class Test;

namespace detail
{
    inline void log_failed_functions();
    template<class T> inline int register_fixture(const String& class_name, const String& test_name);
}

/*!
 * @brief Base class used to define fixtures
 */
class Test
{
    friend void detail::log_failed_functions();
    template<class T> friend  int detail::register_fixture(const String& class_name, const String& test_name);

    friend void run_fixtures();

public:

    /*!
     * @brief Override this function to initialize the fixture resources
     */
    virtual void set_up(){}

    /*!
     * @brief Override this function to release the fixture resources
     */
    virtual void tear_down(){}
    virtual ~Test()=default;

private:

    String _class_name;
    String _test_name;
    
    /*!
     * @brief Overriden by the TEST_F macro
     */
    virtual void run(){}
};

template<class T>
class Equals 
{
public:

    Equals(const T v1) : _val1(v1){}

    template<class U>
    bool run( U val2){return this->_val1 == val2;}

    T _val1;
};

template<class T>
class NonEquals 
{
public:
    
    NonEquals( T v1) : _val1(v1){}

    template<class U>
    bool run( U val2){return this->_val1 != val2;}

    T _val1;
};

template<class T>
class AlmostEquals 
{
public:

    AlmostEquals(T v1,T precision) : _val1(v1), precision(precision){}

    bool run(T val2) 
    {
        return (this->_val1> (val2 - precision)) && (this->_val1 < (val2 + precision));
    }
    
    T precision;
    T _val1;
};

// These functions allows us to write equals(4) instead of Equals<int>(4) 
template<class T> UniquePtr<Equals<T>> equals(T val){return std::make_unique<Equals<T>>(val);}
template<class T> UniquePtr<NonEquals<T>> non_equals(T val){ return std::make_unique<NonEquals<T>>(val); }
template<class T> UniquePtr<AlmostEquals<T>> almost_equals(T val, T precision){return std::make_unique<AlmostEquals<T>>(val, precision);}

namespace detail 
{
// Typedef/Using/Classes
using TestFunctionPointer = void(*)();
template<class T> using Vector = std::vector<T>;
template<class T, class U> using Map = std::map<T,U>;

enum class ConsoleColor
{
    Black, Red, Green, Yellow, Blue, Magenta, Cyan, White
};

class TestFunction
{
public:

    TestFunction(TestFunctionPointer ptr, const String& name, const String& group):
        pointer(ptr),
        name(name),
        group(group){}

    TestFunctionPointer pointer;
    String name;
    String group;
    String file;
    int                 line;
};

// Variables 

inline Map<String, Vector<TestFunction>> map_test_functions;
inline Map<String, Vector<std::unique_ptr<Test>>> fixtures_map;
inline Vector<Test*> failed_fixtures;
inline Vector<TestFunction> failed_functions;

inline int error{0};

inline ConsoleColor current_color{ ConsoleColor::White};

const inline std::map<ConsoleColor,String> color_code // can't constexpr sadly
{
    {ConsoleColor::Black,"30m"}, 
    {ConsoleColor::Red, "31m"},
    {ConsoleColor::Green, "32m"},
    {ConsoleColor::Yellow, "33m"},
    {ConsoleColor::Blue, "34m"},
    {ConsoleColor::Magenta, "35m"},
    {ConsoleColor::Cyan, "36m"},
    {ConsoleColor::White, "37m"},
};

// Some code are probably wrong here
const inline std::map<ConsoleColor,int> win_color_code 
{
    {ConsoleColor::Black,   1}, 
    {ConsoleColor::Red,     4},
    {ConsoleColor::Green,   2},
    {ConsoleColor::Yellow,  14},
    {ConsoleColor::Blue,    5},
    {ConsoleColor::Magenta, 6},
    {ConsoleColor::Cyan,    3},
    {ConsoleColor::White,   8},
};

/*!
 * @brief Just a shortcut so I don't have to write std::cout<< text << "\n" all the time
 */
inline void write_line(const String& str)
{
    std::cout<<"\033[0;"<<color_code.at(current_color)<<str.c_str()<<"\033[0m"<<std::endl;
}

inline void write(const String& str)
{
    std::cout<<"\033[0;"<<color_code.at(current_color)<<str.c_str()<<"\033[0m"<<std::flush;
}

inline void write(const String& str, ConsoleColor code_color)
{
    current_color = code_color;
    write(str);
}

/*!
 * @brief  Write a line of text and changes the console color
 * @param line         Contains the text to be displayed
 * @param console_color   Contains a code corresponding to a color
 */
inline void write_line(const String& line, ConsoleColor console_color)
{
    current_color = console_color;
    write_line(line);
}

template<class T>
void log_test_error(const T val, const String& value_name, const String& expected, const char* file, int line)
{
    write_line("\n        ! Error : ",ConsoleColor::Red);
    write("            * file :     ", ConsoleColor::Cyan);
    write_line(file, ConsoleColor::Yellow);
    write("            * line :     ", ConsoleColor::Cyan);
    write_line(std::to_string(line), ConsoleColor::Magenta);
    write("            * Check if ", ConsoleColor::Cyan);
    write("\"" + value_name + "\" ", ConsoleColor::Magenta);
    write("== ");
    write_line("\"" + expected + "\"");
    write("                * Expected : ", ConsoleColor::Cyan);
    write_line(expected, ConsoleColor::Magenta);
    write("                * Value is : ", ConsoleColor::Cyan);

    std::stringstream ss;
    ss << val;
    write_line(ss.str(), ConsoleColor::Magenta);
    error += 1;
}

/*!
 * @brief 
 */
template<class T, class U>  // Maybe the function signature could be smaller 
void assert_that_(T val, U&& checker,const String& value, const String& expected, const char* file, int line)
{
    if (checker->run(val) == false)
    {
        log_test_error(val, value, expected, file, line);
    }
}

/*!
 * @brief      Register a test function
 *  Called by the TEST macro.  The TEST macro declares a function 
 *  that will be named from the combination of the @ref group_name and @ref
 *  function_name parameters.
 *  Then, it will create a unique variable whose only goal is to call
 *  the register_function with a pointer to the previously declared function, along
 *  with the function name and the group (parameters of the TEST macro)
 *  Finally, it will start the function definition.
 *  @param func             Pointer to the test function created by the TEST macro
 *  @param group_name       First parameter of the TEST macro. Correspond to the group
 *                          in which the function belong
 *  @param function_name    Second parameter of the TEST macro. Correspond to the 
 *  function name.
 */
inline int register_function(TestFunctionPointer func_ptr,const String& function, const String& group)
{
    map_test_functions[group].emplace_back(func_ptr,function,group);
    return 0; // We only return a value because of the affectation trick in the macro
}

/*!
 *   @brief Register a fixture object
 */
template<class T>
inline int register_fixture(const String& class_name, const String& test_name)
{
    try
    {
        T* t = new T();
        t->_class_name = class_name;
        t->_test_name  = test_name;

        fixtures_map[class_name].emplace_back(t); 
        return 0; // We only return a value because of the affectation trick in the macro
    }
    catch(const std::exception& e)
    {
        std::cout<<" ERROR " << e.what() << std::endl;
        return 0;
    }
}

/*!
 * @brief  Logs every failed function inside the console as a summary report
 */
inline void log_failed_functions()
{
    for (const auto& t : detail::failed_functions)
    {
        write_line("      * Function "+ t.group + "::" + t.name+" failed", ConsoleColor::Red);
    }

    for(const auto& t : detail::failed_fixtures)
    {
        write_line("      * "+t->_class_name+"::"+t->_test_name+" failed", ConsoleColor::Red);
    }
}        

/*!
 * @brief  Logs that we successfully pass every test
 */
inline void log_success()
{
    write_line("    * Every test passed", ConsoleColor::Green);
}

/*!
 * @brief  Logs that at least 1 test failed
 */
inline void log_failure()
{
    write_line("    Error : Some test failed to pass", ConsoleColor::Red);
    write_line("    Logging the Functions that failed", ConsoleColor::Cyan);
    log_failed_functions();
}

/*!
 *   @brief  Write an header in the console
 *   @detail Just write something in that way inside the console
 *   ***********
 *   *   Text  *  
 *   ***********
 */
inline void write_title(const String& text)
{
    const int max_column = 78;

    write_line(String(max_column,'*'), ConsoleColor::Green);
    write_line("*    "+text+String(max_column-1 - (5+text.size()), ' ')+"*");
    write_line(String(max_column,'*'));
}

/*!
 *   @brief  Logs that we're running tests that belongs to the @ref group_name group
 *   @param  group_name  Name of the tested group 
 *   @param  group_size  How many test are inside the group
 */
inline void log_start_group(const String& group_name, size_t group_size)
{
    write_title("Running " + std::to_string(group_size) + " tests grouped in " + group_name);
}

/*!
 * @brief Log that we're starting a test
 * @param test_name    Name of the test we're running
 * @param group_name   Name of the group the test belongs to
 * @param count        Counter used to know on which test from group we're (like
 * the first one, the second one ? etc
 */
inline void log_start_test(const String& test_name, const String& group_name, size_t group_size, size_t count)
{
    write("  * Running ", ConsoleColor::Cyan);
    write( group_name+"."+test_name, ConsoleColor::Yellow);
    write(" (" + std::to_string(count) + "/" + std::to_string(group_size) + ")", ConsoleColor::Cyan);
    std::cout<<std::endl;
}

/*!
 * @brief  Log that a test was successful
 */
inline void log_test_success(long long time)
{
    write_line("       Passed in " + std::to_string(static_cast<double>(time) / 1000.0) + " ms", ConsoleColor::Green);
}

/*!
 * @brief  Logs all the results
 */
inline void log_results()
{
    write_title("Results");
    // Log success or failure depending on the error count
    ( detail::error == 0 ) ? detail::log_success() : detail::log_failure();
}
}

// register the time it takes for a function to run
inline long long function_time(std::function<void()> fun)
{
    const auto start = std::chrono::high_resolution_clock::now();
    fun();
    const auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

inline void run_fixtures()
{
    for (auto& fixture : detail::fixtures_map) // Loop through every fixture
    {
        auto total_test = detail::fixtures_map[fixture.first].size();
        int test_index{1};
  
        detail::log_start_group(fixture.first, total_test);

        for (auto& test : fixture.second) // loop through every fixture's test
        {
            detail::log_start_test(test->_test_name, test->_class_name, total_test, test_index++);

            // The assert function will increment the error count if something
            // went wrong. So we just registered how many error we had before
            // running the fixture, and compare afterward to know if the 
            // fixture was a success or not
            int error_value = detail::error;

            test->set_up();
            auto time = function_time([&](){test->run();});
            test->tear_down();
            
            if (error_value == detail::error)
            {
                detail::log_test_success(time);
            }
            else
            {
                //detail::get_failed_fixtures().push_back(test_object);
            }
            std::cout<<std::flush;
        }
    }
}

inline void run_functions()
{
    for (auto test : detail::map_test_functions)
    {
        auto total_test = detail::map_test_functions[test.first].size();
        detail::log_start_group(test.first, total_test);

        int test_index{1};

        for (auto test_function : test.second)
        {
            detail::log_start_test(test_function.name,test_function.group,total_test,test_index++);

            int error_value = detail::error;
            auto time = function_time(test_function.pointer);

            (error_value == detail::error) ? detail::log_test_success(time): detail::failed_functions.push_back(test_function);
        }
    }
}

/*!
 * @brief      Run all the tests defined by the user 
 * @details    Must be called from main. Will fire all the test the user defined
 * with the TEST macro. Warning, this function returns a value that must be returned
 * inside the main function!
 */
inline int run_all()
{
    try
    {
        detail::write_line("RUN ALL THE THINGS!!!", detail::ConsoleColor::Green);
        run_fixtures();
        run_functions();
        detail::log_results();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return detail::error;   // Must return 0 to pass
}

/*!
 *   @brief  Define a new Fixture
 *
 *   A fixture is simply a way to organize tests around a class.
 *   To create a fixture, the user first needs to create a class
 *   that inherits from @ref corgi::test::Test.
 *   This class comes with 2 virtual methods the user needs to override :
 *   set_up and tear_down.
 *   In set_up, the user is expected to initialize the data that will be
 *   used by all the tests of the current feature.
 *   In tear_down, the user is expected to clean the data used by the test.
 *
 *   Once this is done, to add a test to the fixture, the user should use
 *   the TEST_F macro, and define the test. What the framework will do is
 *   call the set_up function, then the code defined inside the TEST_F macro,
 *   and then the tear_down function.
 *
 *   Under the hood, the TEST_F macro will create a new class that inherits from
 *   @ref class_name, declare and define the virtual run method, and register
 *   itself to the framework
 */
#define TEST_F(class_name, test_name)                                                         \
class class_name##test_name : public class_name{ public : void run()override;};                     \
static int var##class_name##test_name = corgi::test::detail::register_fixture<class_name##test_name>(#class_name,#test_name ); \
void class_name##test_name::run()

/*!
 * @brief      Replace the TEST macro with a function registered by the framework
 * @details    Ok so the trick is a little bit dirty, and the parameter should
 * actually not be a string for it to works. The idea is to create an useless variable
 * so I can call the register function. 
 */
#define TEST(group_name,function_name)\
void group_name##_##function_name();   \
static int var##group_name##function_name = corgi::test::detail::register_function(&group_name##_##function_name, #function_name,#group_name); \
void group_name##_##function_name()

#define assert_that(value, expected)  \
    corgi::test::detail::assert_that_(value, expected, #value, #expected, __FILE__, __LINE__)
}}