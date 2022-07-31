#pragma once

#include <corgi/math/Vec3.h>
#include <corgi/math/Line.h>

namespace  corgi::math
{
	
	//  Classe utilisé pour représenté un plan "mathématiquement". Petit rappel, un plan est déterminé soit par
	//  un point P et 2 vecteurs directeurs u et v, ou par un point P et sa normale
	class Plane 
	{

	public:

		// Construction d'un plan à partir d'un point dans l'espace et de 2 vecteurs directeurs.
		// La normale du plan sera construite à partir d'un produit vectoriel des 2 vecteurs directeurs
		Plane(Vec3 point, Vec3 tangent, Vec3 bitangent) 
		{
			m_point		= point;
			m_tangent	= tangent.normalized();
			m_bitangent = bitangent.normalized();
			m_normal	= m_tangent.cross(m_bitangent).normalized();
			compute_coefficients();
		}

		// Construction d'un plan à partir d'un point dans l'espace et de sa normale
		Plane(Vec3 point, Vec3 normal) 
		{
			m_point = point;
			m_normal = normal;
			compute_coefficients();
		}

	// Functions

		// Calcul le point d'intersection entre la droite et le plan. Pour se faire, 
		// On prend l'équation ax+by+cz+d = 0 du plan, et on remplace x,y,z par les valeurs 
		// différentes composantes de l'équation paramétrique de la droite 
		Vec3 line_intersection(Line line) {

			float denom = line.direction().x * m_a + line.direction().y * m_b + line.direction().z * m_c;

			// non Colinarité
			if (denom != 0)
			{
				float t = -line.point().x * m_a - line.point().y * m_b - line.point().z * m_c - m_d;
				t = t / denom;

				return Vec3(
					line.point().x + t * line.direction().x,
					line.point().y + t * line.direction().y,
					line.point().z + t * line.direction().z
				);
			}

			// Dans le cas assez improbable ou la droite et le plan serait colinéaire, on renvoit le point 
			// définit dans la droite
			return line.point();
		}

	private:

	// Functions

		void compute_coefficients() 
		{
			m_a		= m_normal.x;
			m_b		= m_normal.y;
			m_c		= m_normal.z;
			m_d		= -(m_point.x * m_a + m_point.y * m_b + m_point.z * m_c);
		}

	// Variables

		// Coefficients de l'équation du plan ax+by+cz+d=0

		float m_a;
		float m_b;
		float m_c;
		float m_d;

		Vec3 m_point;
		Vec3 m_normal;
		Vec3 m_tangent;
		Vec3 m_bitangent;

		// On recherche l'équation du plan du triangle, on a besoin de la normale pour se faire
		// Rappel, équation du plan : ax+by+cz+d=0
	};
}