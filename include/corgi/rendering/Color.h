#pragma once

#include <cmath>

namespace corgi
{
	struct ColorHSV
	{
		/*!
		 * @brief r,b,g range is 0-255
		 */
		static ColorHSV from_rgb(int red, int blue, int green)
		{
			 // R, G, B values are divided by &255 
	        // to change the range from 0..255 to 0..1 
	        float r = static_cast<float>(red)	/ 255.0f; 
	        float g = static_cast<float>(green)	/ 255.0f; 
	        float b = static_cast<float>(blue)	/ 255.0f; 
	  
	        // h, s, v = hue, saturation, value 
	        float cmax = std::max(r, std::max(g, b)); // maximum of r, g, b 
	        float cmin = std::min(r, std::min(g, b)); // minimum of r, g, b 
	        float diff = cmax - cmin; // diff of cmax and cmin. 
	        float h = -1, s = -1; 
	          
	        // if cmax and cmax are equal then h = 0 
			if (cmax == cmin)
				h = 0;

			// if cmax equal r then compute h 
			else if (cmax == r)
				h = 60.0f * static_cast<float>(std::fmod((g - b) / diff, 6.0f));
	  
	        // if cmax equal g then compute h 
	        else if (cmax == g) 
	            h = 60.0f * ((b - r) / diff + 2) ; 
	  
	        // if cmax equal b then compute h 
	        else if (cmax == b) 
	            h = 60.0f * ((r - g) / diff + 4) ; 
	  
	        // if cmax equal zero 
	        if (cmax == 0.0f) 
	            s = 0.0f; 
	        else
	            s = (diff / cmax) * 100.0f; 
	  
	        // compute v 
	        float v = cmax * 100.0f;

			return {h,s,v};
		}
		
		float hue;
		float saturation;
		float value;
	};
	
}
