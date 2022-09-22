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

    /**
     * @brief Struct used to init a Uniform Buffer Object
     * 
     */
    struct CreateInfo
    {
        ShaderStage shader_stage;
        void*       data;
        int         size;
        int         binding;
    };

    UniformBufferObject(CreateInfo create_info)
        : shader_stage(create_info.shader_stage)
        , data_(create_info.data)
        , size_(create_info.size)
        , binding_(create_info.binding)
    {
    }

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