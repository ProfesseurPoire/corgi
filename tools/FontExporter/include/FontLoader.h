#pragma once

#include <string>

class FontLoader
{
public:
    static bool initialize();
    static void finalize();

    /*!
	 * @brief Exports the ttf file @a path in a format the engine can understand at @a output_path
	 *
	 * @param path	 : Path to the ttf file. Throw an exception if the file isn't a ttf
	 * @param output : Path where the .fnt file will be exported
	 */
    static bool load(const std::string& path, const std::string& output_path);
};