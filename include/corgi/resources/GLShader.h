#pragma once

#include <corgi/resources/Resource.h>
#include <corgi/resources/Shader.h>

#include <string>

namespace corgi
{
class GLShader : public Resource, public Shader
{
public:
    friend class Renderer;
    friend class ShaderProgram;

    enum class Type : unsigned char
    {
        Vertex,
        Fragment
    };

    // Lifecycle

    GLShader(const std::string& path, const std::string& identifier);

    /*
     * @brief   Constructs a new shader by reading the code inside file located @ filepath
     * @param stage     : Shader's type (vertex, fragment, geometric etc)
     * @param filepath  : Location of the file that contains the shader's code
     */
    GLShader(Shader::Stage stage, const std::string& filepath);

    ~GLShader() override;

    GLShader(const GLShader& other) = delete;
    GLShader(GLShader&& other)      = delete;

    GLShader& operator=(const GLShader& other) = delete;
    GLShader& operator=(GLShader&& other) = delete;

    // Functions

    unsigned id() const { return id_; }

    const std::string& source() const { return source_; }

    Type type() const { return type_; }

    [[nodiscard]] long long memory_usage() const override;

private:
    unsigned          id_;
    const std::string source_;
    const std::string name_;
    Type              type_;
};
}    // namespace corgi
