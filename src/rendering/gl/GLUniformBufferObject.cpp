#include <corgi/rendering/gl/GLUniformBufferObject.h>

using namespace corgi;

GLUniformBufferObject::~GLUniformBufferObject()
{
    glDeleteBuffers(1, &buffer_id_);
}

GLUniformBufferObject::GLUniformBufferObject(ShaderStage shader_stage)
    : UniformBufferObject(shader_stage)
{
    glGenBuffers(1, &buffer_id_);
}

void GLUniformBufferObject::set_data(void* data, int size)
{
    data_ = data;
    size_ = size;
    glBindBuffer(GL_UNIFORM_BUFFER, buffer_id_);
    glBufferData(GL_UNIFORM_BUFFER, size, data, GL_DYNAMIC_DRAW);
}

void GLUniformBufferObject::use()
{
    // The binding point is set in the shader
    glBindBufferBase(GL_UNIFORM_BUFFER, 2, buffer_id_);
}

void GLUniformBufferObject::update(int image)
{
    // I probably don't need to update it all the time but it'll work for now
    glBindBuffer(GL_UNIFORM_BUFFER, buffer_id_);
    glBufferData(GL_UNIFORM_BUFFER, size_, data_, GL_DYNAMIC_DRAW);
}
