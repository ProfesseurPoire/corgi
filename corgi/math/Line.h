#pragma once

#include <corgi/math/Vec3.h>

namespace corgi::math
{
	
	/// LineGeometry représente une droite "mathématiquement" Pour rappel, l'équation cartésienne
	/// d'une droite est ax+by+c=0 
	/// 
	/// Quant à l'équation paramétrique, elle s'exprime de la manière suivante : 
	/// X = at + Xa
	/// Y = bt + Ya
	/// Z = ct + Za
	class Line
	{
	public:

	// Lifecycle

		// On définit les paramètres de la courbe à partir d'un point dans l'espace et de son vecteur directeur
		Line(Vec3 point, Vec3 directeur) 
		{
			m_point			= point;
			m_direction		= directeur.normalized();
		}

		// On définit directement les coefficients de l'équation cartésienne
		Line(float a, float b, float c) 
		{
			m_a = a;
			m_b = b;
			m_c = c;
			m_isCartesienne = true;
		}

	// Functions

		// Retourne le coefficient a de la droite ax+by+c=0
		float a() 
		{
			return m_a;
		}

		// Retourne le coefficent b de la droite ax+by+c=0
		float b() {
			return m_b;
		}

		// Retourne le coefficient c de la droite ax+by+c=0
		float c() {
			return m_c;
		}

		// Si la droite à été défini de manière paramétrique, retourne le point P
		Vec3 point() {
			if (m_isCartesienne)
				return Vec3::zero;
			return m_point;
		}

		Vec3 direction() {
			if (m_isCartesienne)
				return Vec3::zero;
			return m_direction;
		}

	private:

		// Variables
				
		// Permet de savoir si la droite à été déterminer de manière cartésienne ou non
		bool m_isCartesienne;

		
		// Coefficients de l'équation cartésienne
		float m_a;
		float m_b;
		float m_c;

		Vec3 m_point;
		Vec3 m_direction;
	};

}