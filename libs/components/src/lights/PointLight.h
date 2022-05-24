#pragma once

#include <corgi/ecs/Component.h>

#include <corgi/math/Vec4.h>
#include <corgi/math/Vec3.h>

namespace corgi
{
	class PointLight : public Component
	{
	public:

		// those are not really meant to be "color" but sets how much the 
		// light will influence the specular, ambient and diffuse value
		// of the material
		Vec3	specular	= Vec3(1.0f, 1.0f, 1.0f) ;
		Vec3	ambient		= Vec3(0.2f, 0.2f, 0.2f) * Vec3(0.5f, 4.0f, 0.5f);
		Vec3	diffuse		= Vec3(0.5f, 0.5f, 0.5f) * Vec3(0.5f, 4.0f, 0.5f);

		// This is for the light's attenuation, see 
		// #https://learnopengl.com/Lighting/Light-casters 


		float constant	=	1.0f;		// almost always to 1.0f
		float linear	=	0.007f;
		float quadratic	=	0.0002f;
	};
}
