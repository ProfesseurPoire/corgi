#version 330 core

in vec2 uv;

uniform sampler2D	main_texture;
uniform vec4 flat_color;
uniform int use_flat_color;
uniform float alpha;

out vec4 color;

void main()
{
	// We discard fragments that are totally transparent
	if(texture( main_texture, uv ).a==0.0)
		discard; 

	if(use_flat_color ==1 )
	{
		color		= flat_color;
	}
	else
	{
		color		= texture( main_texture, uv).rgba;
		color.a = color.a*alpha;
	}
}