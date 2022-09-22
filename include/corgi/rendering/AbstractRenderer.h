#pragma once

#include <corgi/rendering/Material.h>
#include <corgi/rendering/UniformBufferObject.h>
#include <corgi/rendering/texture.h>
#include <corgi/resources/Mesh.h>
#include <corgi/resources/Shader.h>

#include <memory>
#include <vector>

namespace corgi
{
class Window;

/**
 * @brief Base class/interface for rendering operations
 */
class AbstractRenderer
{
public:
    Window* window_ {nullptr};

    /**
     * @brief   Create a texture object
     * 
     *          Textures often needs to be processed/created by the renderer. So, we
     *          need to use this function to create a texture
     * @return  Texture 
     */
    virtual Texture create_texture() = 0;

    /**
     * \brief   Creates a shader object
     * \param   stage
     * \param   file    File with the shader's code 
     * \return 
     */
    virtual Shader* create_shader(Shader::Stage stage, const std::string& file) = 0;

    virtual Sampler* create_sampler(Sampler::CreateInfo info) = 0;

    /**
     * \brief Creates a new material 
     * \param descriptor Struct that contains the data needed to init the material correctly
     * \return 
     */
    virtual AbstractMaterial*
    create_material(AbstractMaterial::Descriptor descriptor) = 0;

    /**
     * \brief Creates a material using the given shader info
     * \param vertex_shader 
     * \param fragment_shader 
     * \return 
     */
    /* virtual AbstractMaterial* create_material(const std::string& vertex_shader_filepath,
        const std::string& fragment_shader_filepath)=0;*/

    /**
     * @brief Delete a texture from the renderer
     */
    virtual void delete_texture(Texture texture) = 0;

    virtual void draw(const Mesh& mesh, const Material& material) = 0;


    virtual AbstractTexture* create_texture(AbstractTexture::CreateInfo info) 
    {
        return nullptr;
    }

    void set_current_window(Window* window) { window_ = window; }

    virtual UniformBufferObject*
    create_ubo(void* data, int size, UniformBufferObject::ShaderStage shader_stage) = 0;

    /**
     * @brief Create a ubo object
     * 
     * @param create_info 
     * @return UniformBufferObject* 
     */
    virtual UniformBufferObject*
    create_ubo(UniformBufferObject::CreateInfo create_info) = 0;

    virtual ~AbstractRenderer() {};

    std::vector<std::unique_ptr<UniformBufferObject>> ubos_;

private:
};

}    // namespace corgi
