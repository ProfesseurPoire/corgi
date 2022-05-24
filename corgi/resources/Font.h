#pragma once

#include <corgi/SimpleString.h>
#include <corgi/containers/UniquePtr.h>
#include <corgi/containers/Vector.h>
#include <corgi/resources/Resource.h>

#include <map>

namespace corgi
{
    class Image;
    class Texture;

    struct CharacterInfo
    {
        int   charCode = 0;    // character's id (utf8)
        float ax, ay;          // advance
        float bw, bh;          // bitmap width and row
        float bl, bt;          // bitmap_left and top
        float tx;              // x offset of glyph in texture coordinates
    };

    /*!
	 * @brief	The font class will read the .fnt file and build a list of available
	 *			configurations. You can use functions like "size" to try to change the current
	 *			configuration for one with the asked sized, but the size might no be available.
	 */
    class Font : public Resource
    {
    public:
        Font(const SimpleString& path, const SimpleString& identifier);

        ~Font() override = default;

        [[nodiscard]] Texture*                            texture();
        [[nodiscard]] const std::map<int, CharacterInfo>& characters() const;
        [[nodiscard]] long
        ascent() const;    // Correspond to the max space of the font on top of the pen
        [[nodiscard]] long descent()
            const;    // Correspond to the max space of the font on the bottom of the pen
        [[nodiscard]] long height() const;    // Correspond to the line height

        [[nodiscard]] int memory_usage() override;

        /*!
		 * @brief Tries to set the size of the font in pixel
		 *
		 *			What it actually does is looks inside the configuration list
		 *			for one that correspond to the current size and other parameters
		 */
        void set_size(int value);

        enum class RenderingMode
        {
            Normal,
            Light,
            Mono,
            LCD,
            LCD_V
        };

        struct Configuration
        {
            int  size {0};
            long ascent {0};
            long descent {0};
            long height {0};

            std::map<int, CharacterInfo> characters;
            UniquePtr<Texture>           texture;
            RenderingMode                rendering_mode;

            // We're using void* instead of hb_font_t here just to avoid
            // including harfbuzz headers here
            void* hb_font;
        };

        Vector<UniquePtr<Configuration>> configurations_;
        int                              current_configuration_index_ {0};
    };
}    // namespace corgi
