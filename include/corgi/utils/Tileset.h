#pragma once

#include <corgi/String.h>

namespace corgi
{
    struct TextureHandle;
}

namespace Editor
{

    struct Tileset
    {
        corgi::String name;
        int width{ 16 };
        int height{ 16 };
        corgi::TextureHandle* texture{ nullptr };
    };
}