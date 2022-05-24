#pragma once


#include <corgi/Ini/Document.h>
#include <corgi/SimpleString.h>

namespace corgi
{
	/*!
	 * @brief	Loads properties from a Setting.ini file located inside the project's resource folder and
	 *			gives an interface to access them from anywhere in the game
	 *			
	 *			A property has a name and a value, and can be grouped into sections
	 */
	class Settings
	{
		friend class Game;
		
	public :

		/*!
		 * @brief	Get the value of the property called @a name 
		 */
		[[nodiscard]] const float&			get_float(const SimpleString& name, const SimpleString& group = "no_section");
		[[nodiscard]] const int&			get_int(const SimpleString& name, const SimpleString& group = "no_section");
		[[nodiscard]] const SimpleString&	get_string(const SimpleString& name, const SimpleString& group = "no_section");

	private:

		SimpleString setting_file_;

		// document where the settings are stored
		ini::Document document_;

		//  Must and can only be initialized by the Game
		void initialize(const SimpleString& path);
		void refresh();
	};
}
