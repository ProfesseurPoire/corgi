#pragma once

#include <corgi/rendering/Sampler.h>

namespace corgi
{
class GLSampler : public Sampler
{
public:
    GLSampler(Sampler::CreateInfo info)
        : Sampler(info)
    {
    }

private:
};
}    // namespace corgi