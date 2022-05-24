#include <corgi/logger/log.h>

#include <vector>
#include <map>
#include <string>

#include <iostream>
#include <fstream>
#include <filesystem>

// Only here so I can have colors on windows
#ifdef _WIN32
#include <windows.h>   
#endif // _WIN32
#include <ctime>

#ifdef _WIN32
// Yoloed that, probably won't work on linux
// Helps with the stack trace
#include <DbgHelp.h>
#pragma comment( lib, "dbghelp.lib" )
#endif


static void set_console_color(int color)
{
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
#endif
}

using namespace corgi;
using namespace std;

struct Channel
{
    // Stores the logs for each log level
    std::map<logger::LogLevel , std::vector<std::string>> logs
    {
        { logger::LogLevel::Info,       std::vector<std::string>() },
        { logger::LogLevel::Trace,      std::vector<std::string>() },
        { logger::LogLevel::Debug,      std::vector<std::string>() },
        { logger::LogLevel::Warning,    std::vector<std::string>() },
        { logger::LogLevel::Error,      std::vector<std::string>() },
        { logger::LogLevel::FatalError, std::vector<std::string>() }
    };
};

static const std::map<logger::LogLevel, std::string> log_level_str
{
    {logger::LogLevel::Info,        "Info"},
    {logger::LogLevel::Trace,       "Trace"},
    {logger::LogLevel::Debug,       "Debug"},
    {logger::LogLevel::Warning,     "Warning"},
    {logger::LogLevel::Error,       "Error"},
    {logger::LogLevel::FatalError,  "FatalError"}
};

static const std::map<corgi::logger::LogLevel, int> color_code
{
    {logger::LogLevel::Info,        11},
    {logger::LogLevel::Trace,       11},
    {logger::LogLevel::Debug,       11},
    {logger::LogLevel::Warning,     14},
    {logger::LogLevel::Error,       12},
    {logger::LogLevel::FatalError,  12}
};

static std::map<std::string, Channel>		channels_;
static bool show_time_ {true};
static bool write_logs_in_console_  {true};
static bool write_logs_in_file_     {true};

static std::string output_folder_{"logs"};
static std::map<std::string, std::ofstream> files_;

// Set that to false if you don't want the log operations to write
// inside a file

void logger::toggle_file_output(const bool value)
{
    write_logs_in_file_ = value;
}

void logger::toggle_console_output(const bool value)
{
    write_logs_in_console_=value;
}

void logger::show_time(const bool v)
{
    show_time_ = v;
}

void logger::set_folder(const std::string& path)
{
    output_folder_ = path;
}

static std::string filename(const std::string& path)
{
    for (size_t i = path.size() - 1; i > 0; --i)
    {
        if (path[i] == '/' || path[i] == '\\')
        {
            return path.substr(i + 1, std::string::npos);	//npos means until the end of the string
        }
    }
    return "";
}
 
void logger::close_files()
{
    for(auto& file : files_)
    {
        file.second.close();
    }
}

std::string build_string(corgi::logger::LogLevel log_level, int line, const std::string& file, const std::string& func, const std::string&text, const std::string& channel)
{
    return  log_level_str.at(log_level) + " : {" +channel + "} : \"" + text +"\" at (" + filename(file) + "::" + func + " " + std::to_string(line) +  ") \n";
}

std::string get_time()
{
    // Probably should make a function for that 
	auto time	= std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	auto gmtime     = std::gmtime(&time);

    std::string minutes = std::to_string(gmtime->tm_min);
    if(gmtime->tm_min<10)
    {
        minutes = "0"+minutes;
    }

    std::string seconds =  std::to_string(gmtime->tm_sec);
    if(gmtime->tm_sec<10)
    {
        seconds = "0"+seconds;
    }

    return (std::to_string(gmtime->tm_hour) + ":" +minutes+ ":" +seconds);
}

void logger::details::write_log(const std::string& obj, const LogLevel log_level, const std::string& channel, const std::string& file, const std::string& func, const int line)
{
    auto str = build_string(log_level, line, file, func, obj, channel);

    set_console_color(color_code.at(log_level));

    if(show_time_)
    {
        str =  "["+ get_time() +"] : " + str;
    }
	
    if(write_logs_in_console_)
    {
        std::cout << str << std::flush;
    }

    channels_[channel].logs.at(log_level).push_back(str);

    if (write_logs_in_file_)
    {
        if (!files_[channel].is_open())
        {
            //Creates the directory to store the logs if it doesn't exist already
            std::filesystem::create_directory(output_folder_);

            // Opening/closing to erase the content of the file, a bit weird 
            // but if I open the file with trunc it won't write on it
            files_[channel].open((output_folder_ +"/"+ channel + ".log"), std::ofstream::out | std::ofstream::trunc);
            files_[channel].close();
            files_[channel].open((output_folder_ +"/"+ channel + ".log"), std::ofstream::out | std::ofstream::app );
        }
        if (files_[channel].is_open())
        {
            files_[channel] << str;
        }
    }

    if(log_level == logger::LogLevel::FatalError || log_level== logger::LogLevel::Error)
    {
        #ifdef _WIN32

        // Actually show the stack 

        void* stack[200];
        HANDLE process = GetCurrentProcess();
        SymInitialize(process, NULL, TRUE);
        WORD numberOfFrames = CaptureStackBackTrace(0, 200, stack, NULL);
        SYMBOL_INFO* symbol = (SYMBOL_INFO*)malloc(sizeof(SYMBOL_INFO) + (200 - 1) * sizeof(TCHAR));
        symbol->MaxNameLen = 200;
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        DWORD displacement;
        IMAGEHLP_LINE64* line = (IMAGEHLP_LINE64*)malloc(sizeof(IMAGEHLP_LINE64));
        line->SizeOfStruct = sizeof(IMAGEHLP_LINE64);
        for (int i = 0; i < numberOfFrames; i++)
        {
            DWORD64 address = (DWORD64)(stack[i]);
            SymFromAddr(process, address, NULL, symbol);
            if (SymGetLineFromAddr64(process, address, &displacement, line))
            {
                printf("\tat %s in %s: line: %lu: address: 0x%0X\n", symbol->Name, line->FileName, line->LineNumber, static_cast<unsigned int>(symbol->Address));
            }
            else
            {
                /*printf("\tSymGetLineFromAddr64 returned error code %lu.\n", GetLastError());
                printf("\tat %s, address 0x%0X.\n", symbol->Name, symbol->Address);*/
            }
        }
    #endif
    }
}