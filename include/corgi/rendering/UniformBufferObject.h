#pragma once

namespace corgi
{
/**
 * @brief Data sent to the Shaders
 * 
 *        The UniformBufferObject doesn't directly stores the data sent, just   
 */
class UniformBufferObject
{
public:
    enum class ShaderStage
    {
        Vertex,
        Fragment
    };

    UniformBufferObject(ShaderStage shader_stage)
        : shader_stage(shader_stage)
    {
    }

    virtual ~UniformBufferObject() = default;

    virtual void set_data(void* data, int size) = 0;

    virtual void use() = 0;

    // the image thing is mostly for vulkan since it can have more than one buffer
    // depending on the current image being processed
    virtual void update(int image = 0) = 0;

    // Id to the buffer used
    int buffer_id;

    void* data_ = nullptr;
    int   size_ = 0;

    // Position of the Uniform block in the shader
    int binding_ = 0;

    ShaderStage shader_stage;

private:
};

}    // namespace corgi