#pragma once

#include <corgi/resources/Resource.h>
#include <corgi/SimpleString.h>

namespace corgi
{
	class Shader : public Resource
	{
	public:
		
		friend class Renderer;
		friend class ShaderProgram;

		enum class Type : unsigned char{ Vertex, Fragment};

	// Lifecycle

		Shader(const SimpleString& path, const SimpleString& identifier);
		~Shader() override;

		Shader(const Shader& other)=delete;
		Shader(Shader&& other)=delete;

		Shader& operator=(const Shader& other)=delete;
		Shader& operator=(Shader&& other)=delete;

	// Functions

		unsigned		id()const			{return id_;}
		const SimpleString&	source()const	{return source_;}
		Type				type()const		{return type_;}

		int memory_usage() override;
	
	private:

		unsigned id_;
		const SimpleString source_;
		const SimpleString name_;
		Type type_;
	};
}	
