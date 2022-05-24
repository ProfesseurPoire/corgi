#include "RenderCommand.h"

#include <glad/glad.h>

// The log system needs a whole polish
#include <corgi/logger/log.h>
#include <corgi/rendering/texture.h>

// Made this a macro so the when I log the error, I actually knows who called what
#define check_gl_error()                                        \
    {                                                           \
        GLenum result;                                          \
                                                                \
        while((result = glGetError()) != GL_NO_ERROR)           \
        {                                                       \
            switch(result)                                      \
            {                                                   \
                case GL_INVALID_ENUM:                           \
                    log_error("Invalid Enum");                  \
                    break;                                      \
                                                                \
                case GL_INVALID_VALUE:                          \
                    log_error("Invalid Value");                 \
                    break;                                      \
                                                                \
                case GL_INVALID_OPERATION:                      \
                    log_error("Invalid Operation");             \
                    break;                                      \
                                                                \
                case GL_INVALID_FRAMEBUFFER_OPERATION:          \
                    log_error("Invalid Framebuffer Operation"); \
                    break;                                      \
                                                                \
                case GL_OUT_OF_MEMORY:                          \
                    log_error("Out of Memory");                 \
                    break;                                      \
                default:                                        \
                    break;                                      \
            }                                                   \
        }                                                       \
    }

namespace corgi
{
    unsigned int RenderCommand::generate_buffer_object()
    {
        GLuint buffer_object;
        glGenBuffers(1, &buffer_object);
        check_gl_error();
        return buffer_object;
    }

    void RenderCommand::bind_index_buffer_object(unsigned id)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
    }

    void RenderCommand::bind_vertex_buffer_object(unsigned id)
    {
        glBindBuffer(GL_ARRAY_BUFFER, id);
    }

    void RenderCommand::delete_vertex_array_object(unsigned id)
    {
        glDeleteVertexArrays(1, &id);
    }

    void RenderCommand::check_error() { check_gl_error(); }

    void
    RenderCommand::buffer_vertex_data(unsigned int index, const float* data, int size)
    {
        glBindBuffer(GL_ARRAY_BUFFER, index);
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    }

    void
    RenderCommand::buffer_vertex_subdata(unsigned int index, const float* data, int size)
    {
        glBindBuffer(GL_ARRAY_BUFFER, index);
        glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
    }

    void RenderCommand::buffer_index_data(unsigned int        index,
                                          const unsigned int* data,
                                          int                 size)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    }

    void RenderCommand::buffer_index_subdata(unsigned int        index,
                                             const unsigned int* data,
                                             int                 size)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, size, data);
    }

    void RenderCommand::vertex_attribute_pointer(unsigned id,
                                                 unsigned stride,
                                                 int      offset,
                                                 unsigned size)
    {
        glVertexAttribPointer(id, size, GL_FLOAT, GL_FALSE, stride,
                              (void*)(offset * sizeof(GL_FLOAT)));
        check_gl_error();
    }

    void RenderCommand::enable_vertex_attribute(unsigned id)
    {
        glEnableVertexAttribArray(id);
        check_gl_error();
    }

    void RenderCommand::disable_vertex_attribute(unsigned id)
    {
        glDisableVertexAttribArray(id);
    }

    void RenderCommand::delete_vertex_buffer_object(unsigned int index)
    {
        glDeleteBuffers(1, &index);
        check_gl_error();
    }
    unsigned RenderCommand::generate_vao_buffer()
    {
        unsigned id;
        glGenVertexArrays(1, &id);
        return id;
    }

    void RenderCommand::bind_vertex_array(unsigned id) { glBindVertexArray(id); }

    void RenderCommand::set_color_attachement(unsigned texture)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                               texture, 0);
    }

    void RenderCommand::set_depth_stencil(unsigned texture)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D,
                               texture, 0);
    }

    unsigned int RenderCommand::generate_frame_buffer()
    {
        unsigned int id;
        glGenFramebuffers(1, &id);
        return id;
    }

    void RenderCommand::delete_frame_buffer(unsigned id) { glDeleteFramebuffers(1, &id); }

    void RenderCommand::bind_frame_buffer(unsigned id)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, id);
    }

    void RenderCommand::delete_texture_object(unsigned int index)
    {
        glDeleteTextures(1, &index);
    }

    void RenderCommand::begin_texture(const Texture* texture)
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture->id_);
        check_gl_error();
    }

    unsigned int RenderCommand::generate_texture_object()
    {
        unsigned int id;
        glEnable(GL_TEXTURE_2D);
        glGenTextures(1, &id);
        check_gl_error();
        return id;
    }

    // TODO : remove check_gl_error in release mode
    void RenderCommand::bind_texture_object(unsigned int id)
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, id);
        check_gl_error();
    }

    void RenderCommand::texture_parameter(Texture::MagFilter filter)
    {
        switch(filter)
        {
            case Texture::MagFilter::Nearest:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                break;
            case Texture::MagFilter::Linear:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                break;
        }
        check_gl_error();
    }

    void RenderCommand::texture_parameter(Texture::MinFilter filter)
    {
        switch(filter)
        {
            case Texture::MinFilter::Nearest:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                break;
            case Texture::MinFilter::Linear:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                break;
            case Texture::MinFilter::NearestMipmapNearest:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                                GL_NEAREST_MIPMAP_NEAREST);
                break;
            case Texture::MinFilter::NearestMipmapLinear:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                                GL_NEAREST_MIPMAP_LINEAR);
                break;
            case Texture::MinFilter::LinearMipmapLinear:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                                GL_LINEAR_MIPMAP_LINEAR);
                break;
            case Texture::MinFilter::LinearMipmapNearest:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                                GL_LINEAR_MIPMAP_NEAREST);
                break;
        }
        check_gl_error();
    }

    void RenderCommand::texture_wrap_s(Texture::Wrap wrap)
    {
        switch(wrap)
        {
            case Texture::Wrap::ClampToBorder:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
                break;

            case Texture::Wrap::ClampToEdge:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                break;

            case Texture::Wrap::MirroredRepeat:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
                break;

            case Texture::Wrap::MirrorClampToEdge:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                                GL_MIRROR_CLAMP_TO_EDGE);
                break;

            case Texture::Wrap::Repeat:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                break;
        }
        check_gl_error();
    }

    void RenderCommand::texture_wrap_t(Texture::Wrap wrap)
    {
        switch(wrap)
        {
            case Texture::Wrap::ClampToBorder:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
                break;

            case Texture::Wrap::ClampToEdge:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                break;

            case Texture::Wrap::MirroredRepeat:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
                break;

            case Texture::Wrap::MirrorClampToEdge:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                                GL_MIRROR_CLAMP_TO_EDGE);
                break;

            case Texture::Wrap::Repeat:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                break;
        }
        check_gl_error();
    }

    void RenderCommand::end_texture()
    {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
    }

    unsigned RenderCommand::create_shader(Shader::Type type)
    {
        switch(type)
        {
            case Shader::Type::Vertex:
                return glCreateShader(GL_VERTEX_SHADER);
                break;
            case Shader::Type::Fragment:
                return glCreateShader(GL_FRAGMENT_SHADER);
                break;
        }
        return -1;
    }

    void RenderCommand::initialize_texture_object(Texture::Format         f,
                                                  Texture::InternalFormat int_f,
                                                  int                     width,
                                                  int                     height,
                                                  Texture::DataType       dt,
                                                  void*                   data)
    {

        GLenum format {GL_RGBA};
        GLint  internal_format {GL_RGBA};
        GLenum t {GL_UNSIGNED_BYTE};

        switch(f)
        {
            case Texture::Format::RED:
                format = GL_RED;
                break;
            case Texture::Format::RG:
                format = GL_RG;
                break;
            case Texture::Format::RGB:
                format = GL_RGB;
                break;
            case Texture::Format::BGR:
                format = GL_BGR;
                break;
            case Texture::Format::RGBA:
                format = GL_RGBA;
                break;
            case Texture::Format::BGRA:
                format = GL_BGRA;
                break;
            case Texture::Format::RED_INTEGER:
                format = GL_RED_INTEGER;
                break;
            case Texture::Format::RG_INTEGER:
                format = GL_RG_INTEGER;
                break;
            case Texture::Format::RGB_INTEGER:
                format = GL_RGB_INTEGER;
                break;
            case Texture::Format::BGR_INTEGER:
                format = GL_BGR_INTEGER;
                break;
            case Texture::Format::RGBA_INTEGER:
                format = GL_RGBA_INTEGER;
                break;
            case Texture::Format::BGRA_INTEGER:
                format = GL_BGRA_INTEGER;
                break;
            case Texture::Format::STENCIL_INDEX:
                format = GL_STENCIL_INDEX;
                break;
            case Texture::Format::DEPTH_COMPONENT:
                format = GL_DEPTH_COMPONENT;
                break;
            case Texture::Format::DEPTH_STENCIL:
                format = GL_DEPTH_STENCIL;
                break;

            default:
                break;
        }

        switch(int_f)
        {
            case Texture::InternalFormat::DEPTH_COMPONENT:
                internal_format = GL_DEPTH_COMPONENT;
                break;
            case Texture::InternalFormat::DEPTH_STENCIL:
                internal_format = GL_DEPTH_STENCIL;
                break;
            case Texture::InternalFormat::RED:
                internal_format = GL_RED;
                break;
            case Texture::InternalFormat::RG:
                internal_format = GL_RG;
                break;
            case Texture::InternalFormat::RGB:
                internal_format = GL_RGB;
                break;
            case Texture::InternalFormat::RGBA:
                internal_format = GL_RGBA;
                break;
            case Texture::InternalFormat::R8:
                internal_format = GL_R8;
                break;
            case Texture::InternalFormat::R16:
                internal_format = GL_R16;
                break;
            case Texture::InternalFormat::RG8:
                internal_format = GL_RG8;
                break;
            case Texture::InternalFormat::RG16:
                internal_format = GL_RG16;
                break;
            case Texture::InternalFormat::RG32F:
                internal_format = GL_RG32F;
                break;
            case Texture::InternalFormat::RG32I:
                internal_format = GL_RG32I;
                break;
            case Texture::InternalFormat::RG32UI:
                internal_format = GL_RG32UI;
                break;
            case Texture::InternalFormat::DEPTH24_STENCIL8:
                internal_format = GL_DEPTH24_STENCIL8;
                break;
            default:
                break;
        }

        switch(dt)
        {
            case Texture::DataType::UnsignedByte:
                t = GL_UNSIGNED_BYTE;
                break;
            case Texture::DataType::Byte:
                t = GL_BYTE;
                break;
            case Texture::DataType::UnsignedShort:
                t = GL_UNSIGNED_SHORT;
                break;
            case Texture::DataType::Short:
                t = GL_SHORT;
                break;
            case Texture::DataType::UnsignedInt:
                t = GL_UNSIGNED_INT;
                break;
            case Texture::DataType::Int:
                t = GL_INT;
                break;
            case Texture::DataType::HalfFloat:
                t = GL_HALF_FLOAT;
                break;
            case Texture::DataType::Float:
                t = GL_FLOAT;
                break;
            case Texture::DataType::UnsignedInt24_8:
                t = GL_UNSIGNED_INT_24_8;
            default:
                break;
        }
        glTexImage2D(GL_TEXTURE_2D,
                     0,                  // Level
                     internal_format,    // Internal format
                     width, height,
                     0,         // Border
                     format,    // format
                     t,         // type
                     data       // data
        );

        check_gl_error();
    }
}    // namespace corgi