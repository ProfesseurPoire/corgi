#pragma once

#include <corgi/resources/Resource.h>
#include <corgi/rendering/AbstractTexture.h>

#include <string>

namespace corgi
{
class Image;

//! @brief  Represents an usable OpenGL texture. Must be loaded using an image object
//!
//! Warning, you must make sure to call "use" before using any method
//! modifying the texture's state and when you're done with the texture,
//! don't forget to call close. This is done so we can chain textures
//! operation without having to tell OpenGL to switch texture, which is a
//! costly operation
class Texture : public Resource, AbstractTexture
{
public:
    friend class Renderer;
    friend class RenderCommand;


    // Lifecycle




    Texture();

    Texture(AbstractTexture::CreateInfo info);

    /*!
		 * @brief	Construct a texture from a .tex file
		 */
    Texture(const std::string& path, const std::string& relative_path);

    /*!
		 * @brief	Generates a new texture
		 *			Copies the name
		 */
    Texture(const std::string& name,
            unsigned           width,
            unsigned           height,
            Format             format,
            InternalFormat     internal_format,
            DataType           dt,
            unsigned char*     data = nullptr);

    Texture(Texture&& texture) noexcept;
    Texture(const Texture& texture) = delete;

    // TODO :	Can't copy since the Texture object doesn't hold
    //			the image data on the CPU which is probably
    //			a mistake. Might be easier if it kept a reference
    //			to an image, just so I could for instance update it
    //			or something
    Texture& operator=(Texture&& texture) noexcept;
    Texture& operator=(const Texture& texture) = delete;

    ~Texture() override;

    // Comparisons

    [[nodiscard]] bool operator==(const Texture& other) const noexcept;

    // Exist for sorting purposes
    [[nodiscard]] bool operator<(const Texture& other) const noexcept;

    // Functions

    [[nodiscard]] long long memory_usage() const override;

    /*!
		 *	@brief	Returns the texture's the number returned by the GPU to 
		 *			identify the current Texture
		 */
    [[nodiscard]] unsigned int id() const noexcept;

    [[nodiscard]] unsigned width() const noexcept;
    [[nodiscard]] unsigned height() const noexcept;

    void width(unsigned width) noexcept;
    void height(unsigned height) noexcept;

    /*!
     * @brief  Returns the aspect ratio of the texture
     * The texture ration is width/height
     */
    [[nodiscard]] float ratio() const;

    [[nodiscard]] const char* name() const;

    /*!
		 * @brief Apply the changes done to the texture
		 *
		 *	Sending data to the GPU is a costly operation. So to 
		 *	avoid having to call the GPU each time the user changes one 
		 *	parameter, the user must call this function to commit its 
		 *	changes, thus updating the texture only once
		 *	
		 *	Though I don't really like that I have to remember to call
		 *	this function after playing with a texture
		 */
    void apply_changes();
    void unpack_pixels();

    /*  void tex_sub_image(int mipmap_level, int x, int y, int width, int height,
						   Image::Format, void * pixels);*/

private:
    // Variables

    std::string name_;

    unsigned int id_ = 0u;    // 4 bytes

    // Using unsigned because glTexImage2D uses GLSizei for width/height

    unsigned _width  = 0u;    // 4 bytes
    unsigned _height = 0u;    // 4 bytes

    // Total size : 20 bytes
};
}    // namespace corgi
