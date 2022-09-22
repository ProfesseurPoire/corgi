#pragma once

#include <corgi/rendering/Sampler.h>

namespace corgi
{

class Shader;

class AbstractMaterial
{
public:
    /**
     * \brief This class only exist to 
     */
    struct Descriptor
    {
        Shader*               vertex_shader {nullptr};
        Shader*               fragment_shader {nullptr};
        std::vector<Sampler*> samplers;
    };

    virtual void update() = 0;

private:
};
}    // namespace corgi