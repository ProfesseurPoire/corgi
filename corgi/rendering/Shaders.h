#pragma once

namespace corgi
{
	//	I feel like I probably should do more here, like, actually returning a material with both
	//	the fragment and vertex shader and glsl program

	constexpr char fragment_shader_depth[] = R"(
		#version 330 core
			
			in vec2 uv;

			uniform sampler2D main_texture;

			out vec4 color;

			void main()
			{
				vec4 c = texture( main_texture, uv );
				color = c;
			}
		)";

	/*
	 * Default shader used by the game
	 */
	constexpr char vertex_shader[] = R"(
			#version 330 core
			
			layout(location = 0) in vec3 position;
			layout(location = 1) in vec2 texture_coordinates;
			layout(location = 2) in vec3 normals;

			out vec2 uv;
			out vec3 normal;
			out vec3 fragment_position;

			uniform mat4 model_matrix;
			uniform mat4 view_matrix;
			uniform mat4 projection_matrix;

			void main()
			{
				// We get the normal matrix here
				normal = mat3(transpose(inverse(model_matrix))) * normals;  

				gl_Position = projection_matrix* view_matrix*model_matrix*  vec4(position,1.0);
				fragment_position = (model_matrix * vec4(position, 1.0f)).xyz;
				uv = texture_coordinates;
				
			}	
		)";

	constexpr char unlit_fragment_shader[] = R"(
		#version 330 core
		in vec2 uv;

		uniform sampler2D main_texture;
		uniform int use_texture;

		out vec4 color;
		out vec3 normal;

		void main()
		{
			if(use_texture == 1)
			{
				if(texture( main_texture, uv ).a==0.0)
					discard;

				color = texture(main_texture, uv).rgba;
			}
			else
				color = main_color;
		}
	)";

	// Default fragment shader used by most stuff
	constexpr char fragment_shader[] = R"(
			#version 330 core

			struct Material
			{
				vec3 ambient_color;
				vec3 diffuse_color;
				vec3 specular_color;
				float shininess;
			};

			struct Light
			{
				vec3 position;
				vec3 ambient;
				vec3 diffuse;
				vec3 specular;

				float constant;
				float linear;
				float quadratic;
			};

			in vec2 uv;
			in vec3 normal;
			in vec3 fragment_position;

			uniform Material	material;
			uniform Light		light;

			uniform vec3		camera_position;

			uniform sampler2D	main_texture;
			uniform int			use_texture;
			uniform vec4		main_color;

			out vec4 color;

			void main()
			{
				vec3 l_normal			= normalize(normal);
				vec3 camera_direction	= normalize(camera_position - fragment_position);
				vec3 light_direction	= normalize(light.position - fragment_position);
				vec3 reflect_direction	= reflect(-light_direction, l_normal);  

				float distance    = length(light.position - fragment_position);
				float attenuation = 1.0 / (light.constant + light.linear * distance + 
    		    light.quadratic * (distance * distance));    

				// Ambient

				vec3 ambient = (light.ambient*attenuation) * material.ambient_color;

				// Diffuse
				
				float diff		= max(dot(normal, light_direction), 0.0);
				vec3 diffuse	=  (light.diffuse*attenuation) * ( material.diffuse_color * diff);
				// Specular 

				// 32 here is the shiniess of the object 
				float spec = pow(max(dot(camera_direction, reflect_direction), 0.0), material.shininess);
				vec3 specular = (light.specular*attenuation) * (spec * material.specular_color);
				if(use_texture == 1)
				{
					// We discard fragments that are totally transparent
					if(texture( main_texture, uv ).a==0.0)
						discard; 
					color		= texture( main_texture, uv ).rgba;
					color.rgb		+= (ambient + diffuse + specular);
				}
				else
				{
					color	= vec4((ambient + diffuse + specular),1.0f);
				}
			}
		)";
}