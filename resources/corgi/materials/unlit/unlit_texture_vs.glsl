#version 330 core
			
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texture_coordinates;

out vec2 uv;

uniform mat4 mvp_matrix;

void main()
{
	gl_Position = mvp_matrix *  vec4(position,1.0);
	uv = texture_coordinates;
}	
