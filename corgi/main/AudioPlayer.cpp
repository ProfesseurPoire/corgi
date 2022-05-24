#include "AudioPlayer.h"

#include <SDL2/SDL.h>
#include <SDL_mixer.h>
#include <corgi/logger/log.h>
#include <corgi/resources/Sound.h>

corgi::AudioPlayer::AudioPlayer()
{
    log_info_on("Initializing SDL_Mixer", "SDL");

    //Initialize SDL_mixer
    if(Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 1024 + 512) == -1)
    {
        log_error("Could not initialize SDL mixer");
    }

    log_info_on("SDL_Mixer was sucessfully initialized", "SDL");

    Mix_AllocateChannels(30);
    for(int i = 0; i < 30; i++)
    {
        Mix_Volume(i, MIX_MAX_VOLUME / 8);
    }
}

corgi::AudioPlayer::~AudioPlayer()
{
    Mix_CloseAudio();
    log_info_on("SDL_Mixer was sucessfully finalized", "SDL");
}

void corgi::AudioPlayer::initialize() {}

bool corgi::AudioPlayer::is_playing(int channel) const
{
    return Mix_Playing(channel);
}

void corgi::AudioPlayer::stop_playing(int channel) const
{
    Mix_HaltChannel(-1);
    if(channel == -1)
    {
        for(int i = 0; i < 30; i++)
            Mix_HaltChannel(i);
    }
}

void corgi::AudioPlayer::play(const Sound& sound, int channel)
{
    if(sound.is_wav())
    {
        if(Mix_PlayChannel(channel, static_cast<Mix_Chunk*>(sound.sound_file_), 0) == -1)
        {
            log_error("Could not load wav file");
        }
    }
    else
    {
        if(Mix_PlayMusic(static_cast<Mix_Music*>(sound.sound_file_), -1) == -1)
        {
            log_error("Could not play music file");
        }
    }
}
