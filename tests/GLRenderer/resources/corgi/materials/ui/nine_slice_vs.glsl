#version 330 core
			
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texture_coordinates;

out vec2 uv;

uniform mat4 mvp_matrix;

uniform vec2 offset;
uniform vec2 dimensions;
uniform vec2 pivot;
uniform vec4 borders;	// x = left, y = right,  z = bottom, w = top
uniform vec2 texture_dimensions;

void main()
{
	// sprite.x = width
	// sprite.y = height
	// sprite.z = pivot_value.x
	// sprite.w = pivot_value.y

	// sprite_tex.x = offset_x
	// sprite_tex.y = offset_y
	// sprite_tex.z = texture.width
	// sprite_tex.w = texture.height

	//float offset_texture_x	 	= sprite_tex.x	/ sprite_tex.z;
	//float offset_texture_y	 	= sprite_tex.y	/ sprite_tex.w;

	//float texture_width_coef		= sprite.x		/ sprite_tex.z;
	//float texture_height_coef 	= sprite.y		/ sprite_tex.w;

	//if (flip_x==1)
	//{
	//	offset_texture_x	= offset_texture_x + texture_width_coef ;
	//	texture_width_coef	= - (texture_width_coef);
	//}

	vec3 previous_position = position;
	vec3 pos = vec3(position);

	
	vec2 texc = texture_coordinates;

	if(pos.x == 0.25)
	{
		pos.x 	= offset.x + borders.x;
		texc.x 	= borders.x/texture_dimensions.x;
	}
	else if(pos.x == 0.75)
	{
		pos.x 	= offset.x + dimensions.x - borders.y;
		texc.x 	= 1.0 - (borders.y/texture_dimensions.x);
	}
	else
	{
		pos.x = offset.x + pos.x*dimensions.x;
	}

	if(pos.y == 0.25)
	{
		pos.y 	= offset.y + borders.z;
		texc.y 	= 1.0 - borders.w /texture_dimensions.y;
	}
	else if(pos.y == 0.75)
	{
		pos.y 	= offset.y + dimensions.y - borders.w;
		texc.y 	= borders.z /texture_dimensions.y;
	}
	else
	{
		pos.y = offset.y + pos.y*dimensions.y;
	}

	//vec2 texc = vec2(texture_coordinates);

	//texc.x = offset_texture_x + texc.x * texture_width_coef;
	//texc.y = offset_texture_y + texc.y * texture_height_coef;

	gl_Position = mvp_matrix * vec4(pos,1.0);
	uv = texc;
}	
