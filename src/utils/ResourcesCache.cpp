#include <corgi/filesystem/FileSystem.h>
#include <corgi/rendering/renderer.h>
#include <corgi/rendering/texture.h>
#include <corgi/utils/AsepriteImporter.h>
#include <corgi/utils/ResourcesCache.h>

#include <fstream>
#include <numeric>

namespace corgi
{
void ResourcesCache::loadEverything()
{
    for(const auto& directory : directories_)
    {
        auto files = corgi::filesystem::list_directory(directory.c_str(), true);

        for(auto file : files)
        {
            if(file.extension() == "tex")
            {
                // We first remove the first character
                std::string relativePath =
                    file.path()
                        .substr(directory.size(), file.path().size() - directory.size())
                        .c_str();
                relativePath = relativePath.substr(1);

                relativePath.replace(relativePath.begin(), relativePath.end(), "\\", "/");

                get<Texture>(relativePath);
            }
        }
    }
};

const ResourcesCache::Resources& ResourcesCache::resources()
{
    return resources_;
}

std::string ResourcesCache::find(const std::string& path)
{
    int found_index = -1;

    for(auto i = 0; i < directories_.size(); i++)
    {
        if(filesystem::file_exist((directories_[i] + "/" + path).c_str()))
        {
            found_index = i;
            break;
        }
    }

    if(found_index != -1)
        return directories_[found_index] + "/" + path;

    return "";
}

std::map<std::string, Animation> ResourcesCache::load_animations(const std::string& path)
{
    // Maybe just do all in one go?
    AsepriteImporter importer;

    auto  p       = find(path + ".ani");
    auto* texture = get<Texture>(path + ".tex");

    if(p != "")
    {
        std::ifstream file(p.c_str(), std::ifstream::binary);

        unsigned size;
        file.read(reinterpret_cast<char*>(&size), sizeof size);
        std::vector<CorgiAnimationFrame> frames(size);
        file.read(reinterpret_cast<char*>(&frames.data()[0]),
                  sizeof(CorgiAnimationFrame) * size);

        file.read(reinterpret_cast<char*>(&size), sizeof size);
        std::vector<CorgiActualAnimation> anims(size);
        file.read(reinterpret_cast<char*>(&anims.data()[0]),
                  sizeof(CorgiActualAnimation) * size);

        std::map<std::string, Animation> m;

        for(const auto& anim : anims)
        {
            Animation animation;

            animation.name_ = anim.name;
            animation.looping =
                std::string(animation.name_.c_str()).find("loop") != std::string::npos;

            for(int i = anim.from; i <= anim.to; i++)
            {
                animation.frames.push_back(
                    {Sprite(frames[i].w, frames[i].h, frames[i].x,
                            texture->height() - frames[i].y - frames[i].h, 0.5f, 0.5f,
                            texture),
                     frames[i].duration});
            }
            m.emplace(anim.name, animation);
        }
        return m;
    }

    return std::map<std::string, Animation>();
}

bool ResourcesCache::contains(const std::string& str)
{
    return resources_.contains(str);
}

bool ResourcesCache::is_cached(const std::string& identifier)
{
    return resources_.contains(identifier);
}

std::vector<std::string>& ResourcesCache::directories() noexcept
{
    return directories_;
}

int ResourcesCache::memory_usage()
{
    // TODO : kinda broken anyways
    auto sum {0};
    /*for(const auto& resource : _resources)
                    sum += _resource.second->memory_usage();*/
    return sum;
}

void ResourcesCache::clear() noexcept
{
    resources_.clear();
}
}    // namespace corgi
