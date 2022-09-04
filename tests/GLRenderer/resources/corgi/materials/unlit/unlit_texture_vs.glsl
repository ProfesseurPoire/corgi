#version 430 core
			
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texture_coordinates;

out vec2 uv;

layout(location=2) uniform mat4 mvp_matrix;

void main()
{
	gl_Position = mvp_matrix *  vec4(position,1.0);
	uv = texture_coordinates;
}	
