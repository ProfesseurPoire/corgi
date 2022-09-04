#version 330 core
			
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texture_coordinates;

out vec2 uv;

uniform mat4 mvp_matrix;

uniform vec4 sprite;

uniform int flip_x;

uniform uint texture_width;
uniform uint texture_height;

// This could probably be regrouped into a struct 

uniform uint offset_x;
uniform uint offset_y;

uniform uint sprite_width;
uniform uint sprite_height;

uniform float pivot_x;
uniform float pivot_y;

void main()
{
	float offset_texture_x	 	= float(offset_x)	/ float(texture_width);
	float offset_texture_y	 	= float(offset_y)	/ float(texture_height);
	
	float texture_width_coef	= float(sprite_width)	/ float(texture_width);
	float texture_height_coef 	= float(sprite_height)	/ float(texture_height);

	if (flip_x==1)
	{
		offset_texture_x	= offset_texture_x + texture_width_coef ;
		texture_width_coef	= - (texture_width_coef);
	}

	vec3 pos = vec3(position);

	pos.x = pos.x*float(sprite_width) /2.0 + (pivot_x - 0.5) * float(sprite_width);
	pos.y = pos.y*float(sprite_height) /2.0 - (pivot_y - 0.5) * float(sprite_height);

	vec2 texc = vec2(texture_coordinates);

	texc.x = offset_texture_x + texc.x * texture_width_coef;
	texc.y = offset_texture_y + texc.y * texture_height_coef;

	gl_Position = mvp_matrix *  vec4(pos,1.0);
	uv = texc;
}	
 