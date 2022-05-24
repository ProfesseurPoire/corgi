#pragma once

#include <corgi/SimpleString.h>
#include <corgi/resources/Resource.h>

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
    class Texture : public Resource
    {
    public:
        friend class Renderer;
        friend class RenderCommand;

        enum class MinFilter : char
        {
            Nearest              = 0,
            Linear               = 1,
            NearestMipmapNearest = 2,
            NearestMipmapLinear  = 3,
            LinearMipmapLinear   = 4,
            LinearMipmapNearest  = 5
        };

        enum class MagFilter : char
        {
            Nearest = 0,
            Linear  = 1
        };

        enum class Wrap : char
        {
            ClampToBorder     = 0,
            ClampToEdge       = 1,
            MirroredRepeat    = 2,
            MirrorClampToEdge = 3,
            Repeat            = 4
        };

        enum class Format : char
        {
            RED,
            RG,
            RGB,
            BGR,
            RGBA,
            BGRA,
            RED_INTEGER,
            RG_INTEGER,
            RGB_INTEGER,
            BGR_INTEGER,
            RGBA_INTEGER,
            BGRA_INTEGER,
            STENCIL_INDEX,
            DEPTH_COMPONENT,
            DEPTH_STENCIL
        };

        // TODO : More exist
        enum class InternalFormat : char
        {
            DEPTH_COMPONENT,
            DEPTH_STENCIL,
            RED,
            RG,
            RGB,
            RGBA,
            R8,
            R16,
            RG8,
            RG16,
            RG32F,
            RG32I,
            RG32UI,
            DEPTH24_STENCIL8
        };

        enum class DataType : char
        {
            UnsignedByte,
            Byte,
            UnsignedShort,
            Short,
            UnsignedInt,
            Int,
            HalfFloat,
            Float,
            UnsignedInt24_8
        };

        // Lifecycle

        Texture();

        /*!
		 * @brief	Construct a texture from a .tex file
		 */
        Texture(const SimpleString& path, const SimpleString& relative_path);

        /*!
		 * @brief	Generates a new texture
		 *			Copies the name
		 */
        Texture(const std::string& name,
                unsigned           width,
                unsigned           height,
                MinFilter          min_filter,
                MagFilter          mag_filter,
                Wrap               wrap_s,
                Wrap               wrap_t,
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

        [[nodiscard]] int memory_usage() override;

        /*!
		 *	@brief	Returns the texture's the number returned by the GPU to 
		 *			identify the current Texture
		 */
        [[nodiscard]] unsigned int id() const noexcept;

        /*!
		 * @brief	Returns the filter used when minifying pixels 
		 * @return	Potential values are :
		 *			* MinFilter::Nearest
		 *			* MinFilter::Linear
		 *			* MinFilter::NearestMipmapNearest
		 *			* MinFilter::NearestMipmapLinear
		 *			* MinFilter::LinearMipmapLinear
		 *			* MinFilter::LinearMipmapNearest
		 */
        [[nodiscard]] MinFilter min_filter() const noexcept;

        /*!
		 * @brief	Returns the filter used when magnifying pixels
		 * @return	Potential values are :
		 *			* MagFilter::Linear 
		 *			* MagFilter::Nearest
		 */
        [[nodiscard]] MagFilter mag_filter() const noexcept;

        /*!
		 * @brief Returns the wrap value for the s coordinate
		 */
        [[nodiscard]] Wrap wrap_s() const noexcept;

        /*!
		 * @brief Returns the wrap value for the t coordinate
		 */
        [[nodiscard]] Wrap wrap_t() const noexcept;

        [[nodiscard]] unsigned width() const noexcept;
        [[nodiscard]] unsigned height() const noexcept;

        void width(unsigned width) noexcept;
        void height(unsigned height) noexcept;

        /*!
		 *	@brief Change the filter used for pixel minification
		 *	
		 *	Don't forget to call the @ref apply_changes() function
		 *	after you changed this filter.
		 * 
		 *  @param[in] filter : Potential values are 
		 *						* MinFilter::Nearest
		 *						* MinFilter::Linear
		 *						* MinFilter::NearestMipmapNearest
		 *						* MinFilter::NearestMipmapLinear
		 *						* MinFilter::LinearMipmapLinear
		 *						* MinFilter::LinearMipmapNearest
		 */
        void min_filter(MinFilter filter) noexcept;

        /*! 
		 *	@brief Changes the filter used for pixel magnification
		 * 
		 *  Don't forget to call the @ref apply_changes() function 
		 *	after you changed a filter
		 *  
		 *  @param[in] filter : Potential Values are
		 *						* MagFilter::Nearest 
		 *						* MagFilter::Linear
		 */
        void mag_filter(MagFilter filter) noexcept;

        /*! 
		 *	@brief Changes the way the s coordinate is wrapped
		 *  
		 *  @param[in] wrap :	Values are 
		 *  					* Wrap::ClampToBorder,
		 *  					* Wrap::ClampToEdge,
		 *  					* Wrap::MirroredRepeat,
		 *  					* Wrap::MirrorClampToEdge,
		 *  					* Wrap::Repeat
		 */
        void wrap_s(Wrap wrap) noexcept;

        /*! 
		 *	@brief Changes the way the t coordinate is wrapped
		 * 
		 *  @param[in] wrap :	Values are 
		 *						* Wrap::ClampToBorder,
		 *						* Wrap::ClampToEdge,
		 *						* Wrap::MirroredRepeat,
		 *						* Wrap::MirrorClampToEdge,
		 *						* Wrap::Repeat
		 */
        void wrap_t(Wrap wrap) noexcept;

        /*!
		 * @brief Used by the font to do stuff
		 */
        //void build_texture(int width, int height);

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

        // Todo : I could probably put all of that in half the size but
        // Padding would eat it anyways so
        MinFilter min_filter_ = MinFilter::Nearest;    // 1 byte
        MagFilter mag_filter_ = MagFilter::Nearest;    // 1 byte
        Wrap      wrap_s_     = Wrap::Repeat;          // 1 byte
        Wrap      wrap_t_     = Wrap::Repeat;          // 1 byte

        // Using unsigned because glTexImage2D uses GLSizei for width/height

        unsigned _width  = 0u;    // 4 bytes
        unsigned _height = 0u;    // 4 bytes

        // Total size : 20 bytes
    };
}    // namespace corgi
