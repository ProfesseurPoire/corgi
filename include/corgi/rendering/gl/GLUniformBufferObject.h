#pragma once

#include <corgi/rendering/UniformBufferObject.h>
#include <glad/glad.h>

namespace corgi
{
class GLUniformBufferObject : public UniformBufferObject
{
public:
    GLUniformBufferObject(ShaderStage shader_stage);

    ~GLUniformBufferObject();

    void set_data(void* data, int size) override;
    void use() override;
    void update(int image = 0) override;

    GLuint buffer_id_;

private:
};
}    // namespace corgi