#pragma once

#include <corgi/rendering/Material.h>
#include <corgi/rendering/UniformBufferObject.h>
#include <corgi/rendering/texture.h>
#include <corgi/resources/Mesh.h>

#include <memory>
#include <vector>

namespace corgi
{

/**
 * @brief Base class/interface for rendering operations
 */
class AbstractRenderer
{
public:
    /**
     * @brief   Create a texture object
     * 
     *          Textures often needs to be processed/created by the renderer. So, we
     *          need to use this function to create a texture
     * @return  Texture 
     */
    virtual Texture create_texture() = 0;

    /**
     * @brief Delete a texture from the renderer
     */
    virtual void delete_texture(Texture texture) = 0;

    virtual void draw(const Mesh& mesh, const Material& material) = 0;

    virtual UniformBufferObject*
    create_ubo(UniformBufferObject::ShaderStage shader_stage) = 0;

    virtual ~AbstractRenderer() {};

    std::vector<std::unique_ptr<UniformBufferObject>> ubos_;

private:
};

}    // namespace corgi
