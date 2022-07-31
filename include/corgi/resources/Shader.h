#pragma once

#include <corgi/resources/Resource.h>

#include <string>

namespace corgi
{
class Shader : public Resource
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

    Shader(const std::string& path, const std::string& identifier);
    ~Shader() override;

    Shader(const Shader& other) = delete;
    Shader(Shader&& other)      = delete;

    Shader& operator=(const Shader& other) = delete;
    Shader& operator=(Shader&& other)      = delete;

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
