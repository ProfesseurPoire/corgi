#pragma once

#include <corgi/rendering/AbstractTexture.h>

namespace corgi
{

/**
 * @brief   The sampler sends informations about a texture to the shader
 * 
 *          We also handle everything related to the texture filtering and wrapping
 */
class Sampler
{
public:
    enum class MinFilter : char
    {
        Nearest              = 0,
        Linear               = 1,
        NearestMipmapNearest = 2,
        NearestMipmapLinear  = 3,
        LinearMipmapLinear   = 4,
        LinearMipmapNearest  = 5
    };

    enum class MagFilter : char
    {
        Nearest = 0,
        Linear  = 1
    };

    enum class Wrap : char
    {
        ClampToBorder     = 0,
        ClampToEdge       = 1,
        MirroredRepeat    = 2,
        MirrorClampToEdge = 3,
        Repeat            = 4
    };

    struct CreateInfo
    {
        MinFilter min_filter {MinFilter::Nearest};
        MagFilter mag_filter {MagFilter::Nearest};
        Wrap      wrap_s {Wrap::Repeat};
        Wrap      wrap_t {Wrap::Repeat};
        int       binding;
        AbstractTexture*       texture;
    };

    Sampler(CreateInfo info)
        : min_filter(info.min_filter)
        , mag_filter(info.mag_filter)
        , wrap_s(info.wrap_s)
        , wrap_t(info.wrap_t)
        , binding(info.binding)
        , texture(info.texture)
    {
    }

    MinFilter min_filter {MinFilter::Nearest};
    MagFilter mag_filter {MagFilter::Nearest};

    Wrap wrap_s {Wrap::Repeat};
    Wrap wrap_t {Wrap::Repeat};

    // Where to bind the texture
    int binding;

    // The texture used
    AbstractTexture* texture;

    virtual void use() {}

private:
};
}    // namespace corgi