#version 430 core
			
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texture_coordinates;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

layout(std140, binding = 2) uniform mats
{
	mat4 mvp_matrix;
};

void main()
{
	gl_Position = mvp_matrix *  vec4(position,1.0);
	uv = texture_coordinates;
}