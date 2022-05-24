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
                    auto relativePath = file.path().substr(
                        directory.size(), file.path().size() - directory.size());
                    relativePath = relativePath.substr(1);

                    relativePath = relativePath.replace("\\", "/");
                    get<Texture>(relativePath);
                }
            }
        }
    };

    const ResourcesCache::Resources& ResourcesCache::resources() { return resources_; }

    SimpleString ResourcesCache::find(const SimpleString& path)
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

    std::map<SimpleString, Animation>
    ResourcesCache::load_animations(const SimpleString& path)
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
            Vector<CorgiAnimationFrame> frames(size);
            file.read(reinterpret_cast<char*>(&frames.data()[0]),
                      sizeof(CorgiAnimationFrame) * size);

            file.read(reinterpret_cast<char*>(&size), sizeof size);
            Vector<CorgiActualAnimation> anims(size);
            file.read(reinterpret_cast<char*>(&anims.data()[0]),
                      sizeof(CorgiActualAnimation) * size);

            std::map<SimpleString, Animation> m;

            for(const auto& anim : anims)
            {
                Animation animation;

                animation.name_   = anim.name;
                animation.looping = std::string(animation.name_.c_str()).find("loop") !=
                                    std::string::npos;

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

        return std::map<SimpleString, Animation>();
    }

    bool ResourcesCache::contains(const SimpleString& str)
    {
        return resources_.contains(str);
    }

    bool ResourcesCache::is_cached(const SimpleString& identifier)
    {
        return resources_.contains(identifier);
    }

    Vector<SimpleString>& ResourcesCache::directories() noexcept { return directories_; }

    int ResourcesCache::memory_usage()
    {
        // TODO : kinda broken anyways
        auto sum {0};
        /*for(const auto& resource : _resources)
			sum += _resource.second->memory_usage();*/
        return sum;
    }

    void ResourcesCache::clear() noexcept { resources_.clear(); }
}    // namespace corgi
