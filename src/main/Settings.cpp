#include <corgi/filesystem/FileSystem.h>
#include <corgi/logger/log.h>
#include <corgi/main/Settings.h>

#include <fstream>
using namespace corgi;

void Settings::initialize(const std::string& path)
{
    setting_file_ = path;

    if(!corgi::filesystem::file_exist(path))
    {
        std::ofstream outfile(path.c_str());
    }

    if(!document_.read(path.c_str()))
    {
        //log_fatal_error("Error while reading Setting file \"" + path + "\"");
    }
}

void Settings::refresh()
{
    // Refresh actually doesn't "clear" the current setting thing, which I think is a mistake here
    // cause I'd end up with tons of stuff. I can try to make it smart by saving every new thing and cleaning
    // afterward what's actually not used
    document_.read(setting_file_.c_str());
}

const int& Settings::get_int(const std::string& name, const std::string& group)
{
    return document_.get_int(name.c_str(), group.c_str());
}

const float& Settings::get_float(const std::string& name, const std::string& group)
{
    return document_.get_float(name.c_str(), group.c_str());
}

const std::string& Settings::get_string(const std::string& name, const std::string& group)
{
    return document_.get_string(name.c_str(), group.c_str());
}
