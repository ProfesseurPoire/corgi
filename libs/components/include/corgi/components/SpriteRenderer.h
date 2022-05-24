#pragma once

#include <corgi/math/Vec2.h>
#include <corgi/rendering/Material.h>
#include <corgi/rendering/Sprite.h>
#include <corgi/resources/Mesh.h>

namespace corgi
{
    class Texture;

    /*!
	 * @brief	The SpriteRenderer holds a Sprite object to be rendered
	 */
    class SpriteRenderer
    {
        friend class Renderer;
        friend class SpriteRendererSystem;

    public:
        //  Lifecycle

        /*!
		 * @brief	Delete the mesh used by every sprite renderer
		 */
        static void  release_sprite_mesh();
        static Mesh& get_sprite_mesh();

        /*!
		 * @brief	Construct a default SpriteRenderer 
		 */
        SpriteRenderer();
        SpriteRenderer(Sprite sprite, Pivot pivot = Pivot::Center);

        /*!
		 * @brief	Construct a SpriteRenderer and convert the Texture into a Sprite to be rendered
		 *
		 * @param	texture :	Texture that will be used to create a sprite
		 */
        SpriteRenderer(Texture& texture);

        SpriteRenderer& operator=(SpriteRenderer&& sr) noexcept = default;
        SpriteRenderer& operator=(const SpriteRenderer& sr) = default;

        SpriteRenderer(const SpriteRenderer&)     = default;
        SpriteRenderer(SpriteRenderer&&) noexcept = default;

        int cameraLayer = 1;

        ~SpriteRenderer() = default;

        // Accessors

        void               enable() { _enabled = true; }
        void               disable() { _enabled = false; }
        [[nodiscard]] bool enabled() const { return _enabled; }
        void               enabled(bool val) { _enabled = val; }

        /*!
		 * @brief	Returns a reference to the sprite currently being renderer
		 */
        [[nodiscard]] Sprite& sprite();

        /*!
		 * @brief	Returns the pivot's position
		 *
		 * @return	Returns a Vec2 that contains the pivot's position
		 */
        [[nodiscard]] Vec2 pivot() const noexcept;

        /*!
		 * @brief	Checks if the sprite has been flipped on the x axis
		 *
		 * @return	Returns true if it is flipped, false otherwise
		 */
        [[nodiscard]] bool is_horizontally_flipped() const noexcept;

        /*!
		 * @brief	Checks if the sprite has been flipped on the y axis
		 *
		 * @return	Returns true if it has been flipped, false otherwise
		 */
        [[nodiscard]] bool is_vertically_flipped() const noexcept;

        // Modifiers

        /*!
		 * @brief	Sets the pivot's value
		 *
		 *			The pivot determines how we draw the sprite.
		 *
		 *			For a sprite with width = 16 and height = 16
		 *
		 *		   Center Pivot               Down Pivot            Left Pivot
		 *          (0.5x,0.5y)	              (0.5x,0.0y)		     (0.0x,0.5y)
		 *	 -8x,8y +-------+ 8x,8y  -8x,16y  +-------+	8x,16y	    0x,8y +-------+ 16x,8y
		 *			|       |		  	      |       |		              |       |
		 *			|   +   |		  	      |       |		              +       |
		 *			|       |		  	      |       |		              |       |
		 *	-8x,-8y +-------+ 8x,-8y   -8x,0y +---+---+ 8x,0y      0x,-8y +-------+ 16x,-8y
		 *
		 * @param	pivot : An enum that contains the most commonly used value like Center, Down etc
		 *					this will then be converted in values between the [0.0f,1.0f] range
		 */
        void pivot(Pivot pivot);

        /*!
		 * @brief	Sets the pivot's values
		 *
		 *			The pivot determines how the sprite will be rendered relative
		 *			to the current position (see SpriteRenderer::pivot(Pivot) for
		 *			more information
		 */
        void pivot(float x, float y);

        /*!
		 * @brief	Sets the sprite to be rendered
		 */
        void sprite(Sprite sprite);

        /*!
		 * @brief	Sets the sprite to be rendered by converting the given texture
		 */
        void sprite(Texture& texture);

        /*!
		 * @brief	Flips the sprite on the x axis
		 *
		 *			If the sprite has already been flipped, another call to the
		 *			function will un-flip it
		 */
        void flip_horizontal();

        /*!
		 * @brief	Sets the flip value on the x axis
		 */
        void flip_horizontal(bool value);

        /*!
		 * @brief	Flips the sprite on the y axis
		 *
		 *			If the sprite has already been flipped, another call to the
		 *			function will un-flip it
		 */
        void flip_vertical();

        /*!
		 * @brief	Sets the flip value on the y axis
		 */
        void flip_vertical(bool value);

        const Material& material() const;

        void material(const Material& material);

    private:
        // Variables

        Material _material;
        Sprite   sprite_;
        Vec2     _pivot_value {0.5f, 0.5f};
        bool     _flipped_x {false};
        bool     _flipped_y {false};
        float    _alpha {1.0f};
        bool     _dirty = true;
        bool     _update_horizontal_flip {true};
        bool     _enabled = true;
    };
}    // namespace corgi
