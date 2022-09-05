#pragma once

#include <corgi/rendering/ShaderProgram.h>
#include <corgi/rendering/texture.h>

namespace corgi
{
class Texture;

// Wrap OpenGL commands.
//
// This class exist solely to the purpose of centralizing rendering
// commands so if I should use a different rendering API, I would "just"
// need to update this file
class RenderCommand
{
public:
    // Generates a buffer object that can be used to store Vertex information (VBO)
    // or Index information (IBO)
    static unsigned int generate_buffer_object();

    static void buffer_vertex_data(unsigned int index, const float* data, int size);
    static void buffer_vertex_subdata(unsigned int index, const float* data, int size);
    static void
    buffer_index_data(unsigned int buffer_id, const unsigned int* data, int size);
    static void
    buffer_index_subdata(unsigned int buffer_id, const unsigned int* data, int size);
    static void delete_vertex_buffer_object(unsigned int index);

    static void         delete_texture_object(unsigned int index);
    static void         bind_texture_object(unsigned int id);
    static unsigned int generate_texture_object();
    static void         initialize_texture_object(Texture::Format         format,
                                                  Texture::InternalFormat internal_format,
                                                  int                     width,
                                                  int                     height,
                                                  Texture::DataType       dt,
                                                  void*                   data = 0);

    static void begin_texture(const Texture* texture);
    static void end_texture();

    static unsigned int create_shader(Shader::Type type);

    // static void texture_parameter(Texture::MagFilter filter);
    // static void texture_parameter(Texture::MinFilter filter);
    // static void texture_wrap_s(Texture::Wrap wrap);
    // static void texture_wrap_t(Texture::Wrap wrap);

    static unsigned int generate_frame_buffer();
    static void         delete_frame_buffer(unsigned id);
    static void         bind_frame_buffer(unsigned int id);
    static void         set_color_attachement(unsigned int texture);
    static void         set_depth_stencil(unsigned int texture);

    static void bind_vertex_buffer_object(unsigned id);
    static void bind_index_buffer_object(unsigned id);

    static unsigned int generate_vao_buffer();
    static void         bind_vertex_array(unsigned id);
    static void         delete_vertex_array_object(unsigned id);

    static void
    vertex_attribute_pointer(unsigned id, unsigned stride, int offset, unsigned size);
    static void enable_vertex_attribute(unsigned id);
    static void disable_vertex_attribute(unsigned id);
    static void check_error();
};
}    // namespace corgi