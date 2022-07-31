#pragma once

#include <corgi/math/Vec2.h>

namespace corgi
{
	class Texture;
	
	enum class Pivot : int
	{
		Center		= 0,
		Down		= 1,
		Right		= 2,
		Left		= 3,
		Up			= 4,
		RightUp		= 5,
		LeftUp		= 6,
		LeftDown	= 7,
		RightDown	= 8
	};
	
	/*!
	 * @brief	The Sprite class represents a chunk of a texture that will be rendered
	 *
	 *			This is especially useful when working with atlas textures that contains
	 *			multiple image. This technique is used because changing texture
	 *			is actually quite a costly operation for OpenGL
	 *
	 *		    Texture
	 *		  {0x;texture_height}                        {texture_width; texture_height}
	 *		    +--------------------------------------+
	 *          |								       |
	 *          |			  Sprite                   |
	 *          |		      +-------+ sprite_height  |
	 *          |             |       |                |
	 *          |  offset_x   |       |                |
	 *          |<----------> +-------+ sprite_width   |
	 *          |             ^                        |
	 *			|             | offset_y               |
	 *          |            \ /                       |
	 *			+--------------------------------------+
	 *		  {0x;0y}									{texture_width,0y}
	 */
	class Sprite
	{
	public:
		
	// Lifecycle

		Sprite()=default;
		Sprite(Texture& texture);
		Sprite(Texture* texture);
		Sprite(	unsigned width,		unsigned height,
				unsigned offset_x,	unsigned offset_y,
				float	pivot_x,	float pivot_y, 
			Texture* texture);

	// Functions

		// The pivot thing tells the renderer how he should actually 
		void pivot(Pivot pivot);
		
	// Variables
			
		unsigned width	{0u};
		unsigned height	{0u};

		unsigned offset_x {0u};
		unsigned offset_y {0u};
		
		// TODO : Pivot should probably be in pixel too?
		Vec2 pivot_value {0.5f, 0.5f};

		//  Non owning pointer to the texture
		Texture* texture{nullptr};

		[[nodiscard]] bool operator==(const Sprite& other)const noexcept;
	};
}
