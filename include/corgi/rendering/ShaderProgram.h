#pragma once

#include <corgi/ShortString.h>
#include <corgi/resources/GLShader.h>

namespace corgi
{
    // A shader Program is mainly a combination of shaders
    class ShaderProgram
    {
    public:
        friend class Renderer;

        ShaderProgram() = default;
        ShaderProgram(const char*     name,
                      const GLShader* vertex_shader,
                      const GLShader* fragment_shader,
                      unsigned        id)
            : name_(name)
            , id_(id)
            , _vertex_shader(vertex_shader)
            , _fragment_shader(fragment_shader)
        {
        }

        ~ShaderProgram();

        // Functions

        bool operator<(const ShaderProgram& p) const { return id_ < p.id_; }

        const char* name() const { return name_.c_str(); }

        unsigned int id() const { return id_; }

        // Variables

        ShortString  name_;
        unsigned int id_;

        const GLShader* _vertex_shader;
        const GLShader* _fragment_shader;
    };
}    // namespace corgi
