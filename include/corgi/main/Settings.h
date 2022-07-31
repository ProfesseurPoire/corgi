#pragma once

#include <corgi/Ini/Document.h>

#include <string>

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

public:
    /*!
		 * @brief	Get the value of the property called @a name 
		 */
    [[nodiscard]] const float&       get_float(const std::string& name,
                                               const std::string& group = "no_section");
    [[nodiscard]] const int&         get_int(const std::string& name,
                                             const std::string& group = "no_section");
    [[nodiscard]] const std::string& get_string(const std::string& name,
                                                const std::string& group = "no_section");

private:
    std::string setting_file_;

    // document where the settings are stored
    ini::Document document_;

    //  Must and can only be initialized by the Game
    void initialize(const std::string& path);
    void refresh();
};
}    // namespace corgi
