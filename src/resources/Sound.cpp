#include <SDL_mixer.h>
#include <corgi/logger/log.h>
#include <corgi/resources/Sound.h>

namespace corgi
{
Sound::Sound(const SimpleString& file, const SimpleString& identifier)
    : name_(identifier)
{
    log_info("Constructing sound resource from file : " + file);
    auto end = file.substr(file.size() - 3, 3);
    is_wav_  = end == "wav";

    if(is_wav_)
    {
        sound_file_ = Mix_LoadWAV(file.c_str());
    }
    else
    {
        sound_file_ = Mix_LoadMUS(file.c_str());
    }
}

Sound::~Sound()
{
    log_info("Destructing sound resource " + name_);

    if(is_wav_)
    {
        Mix_FreeChunk((Mix_Chunk*)sound_file_);
    }
    else
    {
        Mix_FreeMusic((Mix_Music*)sound_file_);
    }
}

long long Sound::memory_usage() const
{
    return 0;
}

bool Sound::is_wav() const
{
    return is_wav_;
}
}    // namespace corgi
