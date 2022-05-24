#pragma once

#include <string>
#include <sstream>

// Use the macros to get the File::Func::Line info, and to be able to remove
// the debugging code depending on the CORGI_VERBOSITY preprocessor macro
//
// To change the log_level value, update this in the CMakeLists.txt: 
// target_compile_definitions(<project> PUBLIC CORGI_VERBOSITY=3)

namespace corgi::logger
{
    enum class LogLevel
    {
        Info, Trace, Debug, Warning, Error, FatalError        
    };
}

#if CORGI_VERBOSITY <= 0
#define log_info(text)              logger::log(text, corgi::logger::LogLevel::Info, __LINE__, __FILE__, __func__ , "default")
#define log_info_on(text, channel)  logger::log(text, corgi::logger::LogLevel::Info, __LINE__, __FILE__, __func__ , channel)
#else
#define log_info(text) 
#define log_info_on(test, channel)
#endif

#if CORGI_VERBOSITY <= 1
#define log_trace(text)             logger::log(text, corgi::logger::LogLevel::Trace, __LINE__, __FILE__, __func__ , "default")
#define log_trace_on(text, channel) logger::log(text, corgi::logger::LogLevel::Trace, __LINE__, __FILE__, __func__ , channel)
#else
#define log_trace(text) 
#define log_trace_on(test, channel)
#endif

#if CORGI_VERBOSITY <= 2
#define log_debug(text)             logger::log(text, corgi::logger::LogLevel::Debug, __LINE__, __FILE__, __func__ , "default")
#define log_debug_on(text, channel) logger::log(text, corgi::logger::LogLevel::Debug, __LINE__, __FILE__, __func__ , channel)
#else
#define log_debug(text) 
#define log_debug_on(test, channel)
#endif

#if CORGI_VERBOSITY <= 3
#define log_warning(text)             logger::log(text, corgi::logger::LogLevel::Warning, __LINE__, __FILE__, __func__ , "default")
#define log_warning_on(text, channel) logger::log(text, corgi::logger::LogLevel::Warning, __LINE__, __FILE__, __func__ , channel)
#else
#define log_warning(text) 
#define log_warning_on(test, channel)
#endif

#if CORGI_VERBOSITY <= 4
#define log_error(text)             logger::log(text, corgi::logger::LogLevel::Error, __LINE__, __FILE__, __func__ , "default")
#define log_error_on(text, channel) logger::log(text, corgi::logger::LogLevel::Error, __LINE__, __FILE__, __func__ , channel)
#else
#define log_error(text) 
#define log_error_on(test, channel)
#endif

#if CORGI_VERBOSITY <= 5
#define log_fatal_error(text)             logger::log(text, corgi::logger::LogLevel::FatalError, __LINE__, __FILE__, __func__ , "default")
#define log_fatal_error_on(text, channel) logger::log(text, corgi::logger::LogLevel::FatalError, __LINE__, __FILE__, __func__ , channel)
#else
#define log_fatal_error(text) 
#define log_fatal_error_on(test, channel)
#endif

namespace corgi 
{
/*!
 * @brief  Provides functions to log information about the application
 * 
 * Do not directly use the functions inside the logger namespace.
 * You should use the log_message, log_warning and log_error macros. 
 * This way, you can remove them from the application by switching 
 * the LOG_LEVEL preprocessor macro inside your CMakeLists.txt
 * 
 * ## Log types
 * 
 * The logger makes the distinction between 6 different type of log :
 * 
 * - Info      : Use Info to log general information about the application
 * - Trace     : Use Trace to log detailed information about the application
 * - Debug     : Use Debug to log debugging information about the application
 * - Warning   : Use Warning to log information about a potential error
 * - Error     : Use Error to log information about an error the application can handle
 * - FatalError: Use FatalError to log information about an error the application can't 
 *               recover from
 * 
 * ## Log level
 * 
 * The CORGI_VERBOSITY preprocessor macro is used to specify which logs will be actually
 * registered by the application. Its value ranged from 0 to 4
 * 
 * -0 Everything will be logged by the application
 * -1 Info won't be logged
 * -2 Info + Trace won't be logged
 * -3 Info + Trace + Debug won't be logged
 * -4 Info + Trace + Debug + Warning won't be logged
 * -5 Info + Trace + Debug + Warning + Error won't be logged
 * -6 Nothing will be logged
 * 
 * To set the CORGI_VERBOSITY variable, add this line inside the CMakeLists.txt
 * target_compile_definitions(target PUBLIC CORGI_VERBOSITY=3)
 * 
 * ## Channels
 * 
 * A channel is a string that can be used to sort logs.
 * By default, all log functions are tagged to the "all" channel
 * For instance, imagine you're working on the renderer. When that happens,
 * you probably don't want the console to display logs about the Audio 
 * system. 
 * By tagging the renderer logs with the "renderer" channel, you'll have 
 * the possibility to only watch Renderer logs inside the console
 * by using the set_current_channel method.
 */
namespace logger
{

	///*!
	// * @brief   Logger class won't change much, but it will allow the user
	// *          to control the lifetime of the log machine
	// */
	//class Logger
	//{
	//public:

	//private:
	//};

	
    // Adds a concept here that prevent the user from using an object that doesn't
    // define the << operator (since we're gonna need it in our log function)
    #ifdef __clang__
    #else
    template<typename T>
        concept HasStreamOperator = requires(T x, std::ostream os)
        {
            os << x;
        };
    #endif


namespace details
{
    void write_log(const std::string& obj, LogLevel log_level, const std::string& channel, const std::string& file, const std::string& func, int line);
}

// TODO :   Could also have a "Output" Class, one for ConsoleOutput, one for FileOutput
//          and one for a simple vector?

/*!
 * @brief Toggles if the library should write logs in files
 */
void toggle_file_output(bool value);

/*!
 * @brief Toggles if the library should write logs on the console
 */
void toggle_console_output(bool value);

/*!
 * @brief Closes the files the library is writing to
 */
void close_files();

/*!
 * @brief Toggles time on log messages
 */
void show_time(bool v);

/*!
 * @brief Only display logs tagged with given @p channel
 */
void set_current_channel(const std::string& channel);

/*!
 * @brief Sets the output folder where log files are written to be @p path
 */
void set_folder(const std::string& path);

/*!
 * @brief Logs content into the different outputs
 * @param[in] obj       The content to be displayed
 * @param[in] line      The line from which the function was called
 * @param[in] file      The file from which the function was called
 * @param[in] func      The function that called the function
 * @param[in] channel   The channel on which the message will be displayed
 */
#ifdef __clang__ 
template<class T>
#else
template<HasStreamOperator T>
#endif
void log(   const T& obj, const LogLevel log_level, int line, const std::string& file,
            const std::string& func,  const std::string& channel)
{
    std::stringstream ss;
    ss << obj ;

    logger::details::write_log(ss.str(), log_level, channel, file, func, line);

    // I'm not exactly sure if abort is the right thing to  do here though
    if(log_level == LogLevel::FatalError)
    {
        abort();
    }
}
}
}