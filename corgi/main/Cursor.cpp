#include "Cursor.h"

#include <SDL2/SDL.h>
#include <corgi/containers/Vector.h>

using namespace corgi;

namespace
{
    Cursor::DefaultCursor defaultCursor_;
    Vector<void*>         cursors_;
}

void Cursor::init()
{
    cursors_.reserve(11);
    cursors_.emplace_back(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
    cursors_.emplace_back(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAIT));
    cursors_.emplace_back(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR));
    cursors_.emplace_back(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAITARROW));
    cursors_.emplace_back(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENWSE));
    cursors_.emplace_back(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENESW));
    cursors_.emplace_back(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE));
    cursors_.emplace_back(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS));
    cursors_.emplace_back(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL));
    cursors_.emplace_back(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NO));
    cursors_.emplace_back(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND));
}

void Cursor::finalize()
{
    for(auto& cursor : cursors_)
        SDL_FreeCursor(static_cast<SDL_Cursor*>(cursor));
}

void Cursor::set(DefaultCursor cursor)
{
    defaultCursor_ = cursor;
    SDL_SetCursor
        (
            static_cast<SDL_Cursor*>
            (
                cursors_.at
                (
                    static_cast<int>(cursor)
                    )
            )
            );
}

Cursor::DefaultCursor Cursor::get()
{
    return defaultCursor_;
}
