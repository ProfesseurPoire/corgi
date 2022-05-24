#include "Sprite.h"

#include "texture.h"

namespace corgi
{
	Sprite::Sprite(Texture& texture) :
		width(static_cast<unsigned short>(texture.width())),
		height(static_cast<unsigned short>(texture.height())),
		texture(&texture) 
	{}

	Sprite::Sprite(Texture* texture):
		width(static_cast<unsigned short>(texture->width())),
		height(static_cast<unsigned short>(texture->height())),
		texture(texture)
	{}

	Sprite::Sprite(	
		unsigned width, 
		unsigned height, 
		unsigned offset_x, 
		unsigned offset_y, 
		float pivot_x, 
		float pivot_y, 
		Texture* texture)
		: width(width),height(height), offset_x(offset_x),offset_y(offset_y),pivot_value(pivot_x,pivot_y), texture(texture){}

	void Sprite::pivot(Pivot pivot)
	{
		static Vec2 pivots_[] =
		{
			{Vec2(0.5f, 0.5f)},	// Pivot::Center
			{Vec2(0.5f, 0.0f)},	// Pivot::Down
			{Vec2(1.0f, 0.5f)}, // Pivot::Right
			{Vec2(0.0f, 0.5f)}, // Pivot::Left
			{Vec2(0.5f, 1.0f)},	// Pivot::Up
			{Vec2(1.0f, 1.0f)},	// Pivot::RightUp
			{Vec2(0.0f, 1.0f)}, // Pivot::LeftUp
			{Vec2(0.0f, 0.0f)}, // Pivot::LeftDown
			{Vec2(1.0f, 0.0f)}	// Pivot::RightDown
		};
		
		pivot_value = pivots_[static_cast<int>(pivot)];
	}

	bool Sprite::operator==(const Sprite& other) const noexcept
	{
		if (texture != other.texture)
			return false;

		if (width != other.width)
			return false;

		if (height != other.height)
			return false;

		if (offset_x != other.offset_x)
			return false;

		if (offset_y != other.offset_y)
			return false;

		if (pivot_value != other.pivot_value)
			return false;

		return true;
	}
}
