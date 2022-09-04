#version 430 core

in vec2 uv;

uniform sampler2D	main_texture;
uniform float alpha;

out vec4 color;

void main()
{
	// We discard fragments that are totally transparent
	if(texture( main_texture, uv ).a==0.0)
		discard; 

	color		= texture(main_texture, uv).rgba;
	color.a     = color.a*alpha;
}