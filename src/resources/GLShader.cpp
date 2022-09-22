#include <corgi/filesystem/FileSystem.h>
#include <corgi/logger/log.h>
#include <corgi/resources/GLShader.h>
#include <glad/glad.h>

#include <fstream>

namespace corgi
{
GLShader::GLShader(Shader::Stage stage, const std::string& filepath)
{
    // Creating the shader id
    switch(stage)
    {
        case Shader::Stage::Fragment:
            id_ = glCreateShader(GL_FRAGMENT_SHADER);
            break;
        case Shader::Stage::Vertex:
            id_ = glCreateShader(GL_VERTEX_SHADER);
            break;
    }

    // Getting back the shader code
    auto content = corgi::filesystem::read_file_binary(filepath);

    // Initializing the shader with the code
    glShaderBinary(1, &id_, GL_SHADER_BINARY_FORMAT_SPIR_V, content.data(),
                   content.size());

    glSpecializeShader(id_, (const GLchar*)"main", 0, nullptr, nullptr);

    // Specialization is equivalent to compilation.
    GLint isCompiled = 0;
    glGetShaderiv(id_, GL_COMPILE_STATUS, &isCompiled);

    GLint max_length = 0;

    if(isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(id_, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::string errorLog(max_length, ' ');
        glGetShaderInfoLog(id_, max_length, &max_length, &errorLog[0]);
        log_error(errorLog.c_str());

        // We don't need the shader anymore.
        glDeleteShader(id_);

        // Use the infoLog as you see fit.

        // In this simple program, we'll just leave
        return;
    }

    // // If something went wrong, we log it a little bit
    // if(status == GL_FALSE)
    // {
    //     // We get back the error from OpenGL
    //     GLint max_length = 0;
    //     glGetShaderiv(id_, GL_INFO_LOG_LENGTH, &max_length);

    //     std::string errorLog(max_length, ' ');
    //     glGetShaderInfoLog(id_, max_length, &max_length, &errorLog[0]);

    //     // We log an error but we don't throw because the application
    //     // could theorically still work?
    //     log_error(" Error while loading " + filepath + " shader");
    //     log_error(errorLog.c_str());

    //     glDeleteShader(id_);
    // }
}

GLShader::GLShader(const std::string& path, const std::string& identifier)
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

    auto content = corgi::filesystem::read_file(path);
    auto cstr    = content.c_str();

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

GLShader::~GLShader()
{
    glDeleteShader(id_);
}

long long GLShader::memory_usage() const
{
    // NOT IMPLEMENTED
    // Also not exactly sure what I should be returning here lol
    return 0;
}
}    // namespace corgi
