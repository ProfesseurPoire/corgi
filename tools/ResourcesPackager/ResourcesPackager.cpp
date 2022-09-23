#include "TiledImporter.h"
#include "corgi/resources/image.h"
#include "corgi/utils/AsepriteImporter.h"

#include <FontLoader.h>
#include <corgi/filesystem/FileSystem.h>
#include <corgi/logger/log.h>
#include <corgi/resources/Animation.h>
#include <corgi/utils/Utils.h>
#include <corgi/utils/time/Timer.h>
#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace corgi;

static Vector<corgi::filesystem::FileInfo> get_all_files(const SimpleString& directory)
{
    return corgi::filesystem::list_directory(directory.c_str(), true);
}

static void transform_aseprite_animation(filesystem::FileInfo file_info)
{
    std::ofstream f(
        (file_info.path().substr(0, file_info.path().size() - 5) + ".ani").c_str(),
        std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);

    auto size = filesystem::file_size(file_info.path());

#ifdef __linux__
    FILE* fp = fopen(file_info.path().c_str(), "r");    // non-Windows use "r"
#elif _WIN32
    FILE* fp = fopen(file_info.path().c_str(), "rb");    // non-Windows use "r"
#endif

    char* readBuffer = new char[size];

    rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
    rapidjson::Document       document;
    document.ParseStream(is);

    if(!document.HasMember("frames"))
    {
        delete readBuffer;
        return;
    }

    const auto frame_size = document["frames"].MemberCapacity();

    f.write(reinterpret_cast<const char*>(&frame_size), sizeof frame_size);

    for(auto it = document["frames"].MemberBegin(); it < document["frames"].MemberEnd();
        ++it)
    {
        const auto& value = it->value;

        CorgiAnimationFrame frame;
        frame.duration = static_cast<float>(value["duration"].GetInt());
        frame.x        = static_cast<unsigned short>(value["frame"]["x"].GetInt());
        frame.y        = static_cast<unsigned short>(value["frame"]["y"].GetInt());
        frame.w        = static_cast<unsigned short>(value["frame"]["w"].GetInt());
        frame.h        = static_cast<unsigned short>(value["frame"]["h"].GetInt());
        f.write(reinterpret_cast<const char*>(&frame), sizeof frame);
    }

    const auto frame_tags_size = document["meta"]["frameTags"].GetArray().Size();
    f.write(reinterpret_cast<const char*>(&frame_tags_size), sizeof frame_tags_size);

    for(auto& frame_tag : document["meta"]["frameTags"].GetArray())
    {
        CorgiActualAnimation animation;
        animation.from = frame_tag["from"].GetInt();
        animation.to   = frame_tag["to"].GetInt();

        const auto direction = frame_tag["direction"].GetString();

        if(direction == "forward")
            animation.direction = AnimationDirection::Forward;
        else if(direction == "reverse")
            animation.direction = AnimationDirection::Reverse;
        else if(direction == "pingpong")
            animation.direction = AnimationDirection::PingPong;

        strcpy(animation.name, frame_tag["name"].GetString());

        f.write(reinterpret_cast<const char*>(&animation), sizeof animation);
    }

    delete[] readBuffer;
}

int main(int argc, char** argv)
{
    corgi::logger::set_folder("ResourcePackagerLogs");
    log_info("Resource Packager Process Starting");
    time::Timer timer;
    timer.start();

    FontLoader::initialize();

    Vector<SimpleString> input_directories;

    //input_directories.emplace_back("C:\\dev\\Frog\\resources");

    bool get_directories = false;

    for(int i = 1; i < argc; i++)
    {
        if(std::strcmp(argv[i], "-I") == 0)
        {
            get_directories = true;
            continue;
        }

        if(std::strcmp(argv[i], "-O") == 0)
        {
            get_directories = false;
            continue;
        }

        if(get_directories)
            input_directories.push_back(argv[i]);
    }

    for(auto directory : input_directories)
    {
        std::cout << ("Scanning directory " + directory).c_str() << std::endl;

        for(const auto file : get_all_files(directory))
        {
            if(file.extension() == "ttf")
            {
                std::cout << "Loading TTF file  " + file.path() << std::endl;
                FontLoader::load(file.path(),
                                 file.path().substr(0, file.path().size() - 4) + ".fnt");
            }

            if(file.name().find("tiled.json") != std::string::npos)
            {
                std::cout << "Loading Tiled file  " + file.path() << std::endl;
                auto output = file.path().substr(0, file.path().size() - 10) + "tmp";
                TiledImporter::import(file.path().c_str(), output.c_str());
            }

            if(file.extension() == "anim")
            {
                std::cout << "Loading Anim file  " + file.path() << std::endl;
                transform_aseprite_animation(file);
            }

            if(file.extension() == "png")
            {
                std::cout << "Loading PNG  file  " + file.path() << std::endl;

                // We convert the PNG into a simpler format

                auto* image = Editor::Utils::LoadImageForReal(file.path());

                auto p = file.path().substr(0, file.path().size() - 4) + ".img";

                std::ofstream corgi_image(p.c_str(), std::ofstream::out |
                                                         std::ofstream::trunc |
                                                         std::ofstream::binary);

                auto  w        = image->width();
                auto  h        = image->height();
                auto  channels = image->channel();
                auto* pixels   = image->pixels();

                corgi_image.write(reinterpret_cast<const char*>(&w), sizeof w);
                corgi_image.write(reinterpret_cast<const char*>(&h), sizeof h);
                corgi_image.write(reinterpret_cast<const char*>(&channels),
                                  sizeof channels);

                // the function reading the png file will always convert to rgba
                corgi_image.write(reinterpret_cast<const char*>(pixels), w * h * 4);
                corgi_image.close();

                delete image;

                // We check if a .tex exist for the current png, creates a default
                // one otherwise

                if(!corgi::filesystem::file_exist(
                       file.path().substr(0, file.path().size() - 4) + ".tex"))
                {
                    //Creates a default .tex file

                    std::ofstream f;
                    f.open(
                        (file.path().substr(0, file.path().size() - 4) + ".tex").c_str());

                    f << R"({
	"min_filter"	: "nearest",
	"mag_filter"	: "nearest",
	"wrap_s"		: "repeat",
	"wrap_t"		: "repeat"
})";
                }
            }
        }
    }

    auto time = timer.elapsed_time();

    log_info("Resource Packager Process ended " + std::to_string(time));
}