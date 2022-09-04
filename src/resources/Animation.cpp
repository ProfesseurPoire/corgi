#include <corgi/resources/Animation.h>
#include <corgi/utils/AsepriteImporter.h>

using namespace corgi;

std::map<std::string, Animation> load_animations(const std::string& path)
{
    // Maybe just do all in one go?
    AsepriteImporter importer;

    // auto  p       = find(path + ".ani");
    // auto* texture = get<Texture>(path + ".tex");

    // if(p != "")
    // {
    //     std::ifstream file(p.c_str(), std::ifstream::binary);

    //     unsigned size;
    //     file.read(reinterpret_cast<char*>(&size), sizeof size);
    //     std::vector<CorgiAnimationFrame> frames(size);
    //     file.read(reinterpret_cast<char*>(&frames.data()[0]),
    //               sizeof(CorgiAnimationFrame) * size);

    //     file.read(reinterpret_cast<char*>(&size), sizeof size);
    //     std::vector<CorgiActualAnimation> anims(size);
    //     file.read(reinterpret_cast<char*>(&anims.data()[0]),
    //               sizeof(CorgiActualAnimation) * size);

    //     std::map<std::string, Animation> m;

    //     for(const auto& anim : anims)
    //     {
    //         Animation animation;

    //         animation.name_ = anim.name;
    //         animation.looping =
    //             std::string(animation.name_.c_str()).find("loop") != std::string::npos;

    //         for(int i = anim.from; i <= anim.to; i++)
    //         {
    //             animation.frames.push_back(
    //                 {Sprite(frames[i].w, frames[i].h, frames[i].x,
    //                         texture->height() - frames[i].y - frames[i].h, 0.5f, 0.5f,
    //                         texture),
    //                  frames[i].duration});
    //         }
    //         m.emplace(anim.name, animation);
    //     }
    //     return m;
    // }

    return std::map<std::string, Animation>();
}