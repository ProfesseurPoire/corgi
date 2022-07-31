#pragma once

#include <corgi/String.h>

namespace corgi
{
	class Texture;
	class Image;

    struct TextureHandle
    {
        TextureHandle() {}
        TextureHandle(Texture* texture, const String& name)
            :name(name), texture(texture) {}

        Texture* texture;
        String name;
    };

	struct ImageHandle
	{
		Image* image;
		char* name;
	};
}