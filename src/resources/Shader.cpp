#include <corgi/logger/log.h>
#include <corgi/resources/Shader.h>
#include <glad/glad.h>

#include <fstream>

namespace corgi
{
Shader::Shader(const std::string& path, const std::string& identifier)
{
    //log_info("Shader Constructor for "+path);
    std::string pathh = path.c_str();

    if(pathh.find("_vs") != std::string::npos)
        id_ = glCreateShader(GL_VERTEX_SHADER);
    else if(pathh.find("_fs") != std::string::npos)
        id_ = glCreateShader(GL_FRAGMENT_SHADER);
    else
        throw std::invalid_argument("Could not determine the shader from the path");

    // Loading the source code

    std::ifstream file(path.c_str());
    std::string   code;

    if(!file.is_open())
        throw std::invalid_argument("Could not load shader at : \"" + pathh + "\"");

    file.seekg(0, std::ios::end);
    code.reserve(std::string::size_type(file.tellg()));
    file.seekg(0, std::ios::beg);

    code.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());

    auto cstr = code.c_str();

    // Initializing the shader

    glShaderSource(id_, 1, &cstr, NULL);
    glCompileShader(id_);

    GLint success = 0;
    glGetShaderiv(id_, GL_COMPILE_STATUS, &success);

    if(success == GL_FALSE)
    {
        GLint max_length = 0;
        glGetShaderiv(id_, GL_INFO_LOG_LENGTH, &max_length);

        std::string errorLog(max_length, ' ');

        glGetShaderInfoLog(id_, max_length, &max_length, &errorLog[0]);

        std::string str = " Error while loading " + path + " shader";

        log_error(str.c_str());
        log_error(errorLog.c_str());

        glDeleteShader(id_);
        throw std::invalid_argument("Could not compile shader " + pathh);
    }
}

Shader::~Shader()
{
    glDeleteShader(id_);
}

long long Shader::memory_usage() const
{
    // NOT IMPLEMENTED
    // Also not exactly sure what I should be returning here lol
    return 0;
}
}    // namespace corgi
