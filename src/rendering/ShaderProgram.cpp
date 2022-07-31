#include <corgi/rendering/ShaderProgram.h>

#include <glad/glad.h>

using namespace corgi;

ShaderProgram::~ShaderProgram()
{
	glDeleteProgram(id_);
}
