#include <corgi/main/SDLContext.h>

#include <SDL2/SDL.h>
#include <corgi/logger/log.h>

using namespace corgi;

SDLContext::SDLContext()
{
    log_info_on("Initializing SDL", "SDL");

    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO) != 0)
    {
        log_warning("SDL had an error durring initializiation");
        log_warning(SDL_GetError());
    }
    else
    {
        log_info_on("SDL has been successfully initialized", "SDL");
    }
}

SDLContext::~SDLContext()
{
    SDL_Quit();
    log_info("SDL has been successfully finalized");
}
