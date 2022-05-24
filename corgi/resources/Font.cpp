#include "Font.h"

#include <corgi/logger/log.h>
#include <corgi/rendering/texture.h>
#include <freetype2/ft2build.h>
#include <harfbuzz/hb.h>

#include FT_FREETYPE_H

#include <fstream>
#include <map>
#include <string>

#include <hb-ft.h>

namespace corgi
{
    FT_Library* freetype_library;

    // To avoid texture filtering problems, we don't put glyphs next to each other
    static const int offset_ = 10;

    Texture* Font::texture()
    {
        return configurations_[current_configuration_index_]->texture.get();
    }

    const std::map<int, CharacterInfo>& Font::characters() const
    {
        return configurations_[current_configuration_index_]->characters;
    }

    long Font::ascent() const
    {
        return configurations_[current_configuration_index_]->ascent;
    }

    long Font::descent() const
    {
        return configurations_[current_configuration_index_]->descent;
    }

    long Font::height() const
    {
        return configurations_[current_configuration_index_]->height;
    }

    int Font::memory_usage()
    {
        int sum {0};

        for(auto& configuration : configurations_)
        {
            sum = configuration->texture->memory_usage();
        }

        return sizeof(Font) +
               configurations_.capacity() *
                   (sizeof(std::unique_ptr<Configuration>) + sizeof(Configuration)) +
               sum;
    }

    void Font::set_size(int value)
    {
        int index = 0;

        for(auto& configuration : configurations_)
        {
            if(configuration->size == value)
                current_configuration_index_ = index;
            index++;
        }
    }

    struct Atlas
    {
        CharacterInfo
            characters[256];    // Todo : Not sure this is the best idea I had too
        std::vector<unsigned char> image;
        long                       width {0};
        long                       height {0};
    };

    std::pair<int, int> find_max_dimensions(FT_Face& face)
    {
        const auto* const glyph = face->glyph;

        auto width  = 0;
        auto height = 0;

        // Only scan for the max height
        for(auto i = 32; i < 128; i++)
        {
            if(FT_Load_Char(face, static_cast<unsigned char>(i), FT_LOAD_RENDER))
            {
                printf("Error while Loading character");
            }
            width += glyph->bitmap.width + offset_;
            height = std::max(height, static_cast<int>(glyph->bitmap.rows));
        }
        width -= offset_;

        return {width, height};
    }

    Atlas build_atlas(const Font::Configuration& configuration, FT_Face* face)
    {
        Atlas atlas;

        const auto error = FT_Set_Pixel_Sizes(*face, 0, configuration.size);

        if(error != 0)
            throw std::runtime_error("Error while setting the font pixel size");

        // Btw this is probably not needed since there is something that returns the dimensions
        // already I think inside the font data
        auto [width, height] = find_max_dimensions(*static_cast<FT_Face*>(face));

        const auto* glyph = (*static_cast<FT_Face*>(face))->glyph;

        auto x = 0;

        atlas.image.resize(width * height * 4, 0);    // using vector for raii
        atlas.width  = width;
        atlas.height = height;

        for(int i = 32; i < 128; i++)
        {
            // Get back the glyph index
            auto glyph_index = FT_Get_Char_Index(*face, i);

            // load the glyph
            auto e = FT_Load_Glyph(*face,           /* handle to face object */
                                   glyph_index,     /* glyph index           */
                                   FT_LOAD_RENDER); /* load flags, see below */

            /* if (FT_Load_Char(*face, i, FT_LOAD_RENDER))
    continue;*/

            // set it's rendering mode
            switch(configuration.rendering_mode)
            {
                case Font::RenderingMode::Normal:
                    e = FT_Render_Glyph((*face)->glyph,         /* glyph slot  */
                                        FT_RENDER_MODE_NORMAL); /* render mode */
                    break;

                case Font::RenderingMode::Light:
                    e = FT_Render_Glyph((*face)->glyph,        /* glyph slot  */
                                        FT_RENDER_MODE_LIGHT); /* render mode */
                    break;

                case Font::RenderingMode::Mono:
                    e = FT_Render_Glyph((*face)->glyph,       /* glyph slot  */
                                        FT_RENDER_MODE_MONO); /* render mode */
                    break;

                case Font::RenderingMode::LCD:
                    e = FT_Render_Glyph((*face)->glyph,      /* glyph slot  */
                                        FT_RENDER_MODE_LCD); /* render mode */
                    break;

                case Font::RenderingMode::LCD_V:
                    e = FT_Render_Glyph((*face)->glyph,        /* glyph slot  */
                                        FT_RENDER_MODE_LCD_V); /* render mode */
                    break;
            }

            //auto hasKerning = (!!((*face)->face_flags & FT_FACE_FLAG_KERNING));

            //hb_font_t* hb_font = hb_ft_font_create(*face, NULL);

            // auto hasKerning = FT_HAS_KERNING(*face);

            FT_Vector vector;
            auto      error = FT_Get_Kerning(*face, /* handle to face object */
                                             'L',   /* left glyph index      */
                                             'T',   /* right glyph index     */
                                             FT_KERNING_DEFAULT, /* kerning mode          */
                                             &vector); /* target vector         */

            // Build the information used to know where a character is
            // on the final image
            atlas.characters[i].ax = static_cast<float>(glyph->advance.x >> 6);
            atlas.characters[i].ay = static_cast<float>(glyph->advance.y >> 6);
            atlas.characters[i].bw = static_cast<float>(glyph->bitmap.width);
            atlas.characters[i].bh = static_cast<float>(glyph->bitmap.rows);
            atlas.characters[i].bl = static_cast<float>(glyph->bitmap_left);
            atlas.characters[i].bt = static_cast<float>(glyph->bitmap_top);
            atlas.characters[i].tx = static_cast<float>(x) / static_cast<float>(width);

            // Write the glyphs in the texture
            for(int j = 0; j < glyph->bitmap.rows; j++)
            {
                for(int k = 0; k < glyph->bitmap.width; k++)
                {
                    const int row    = j * width;
                    const int column = (x + k);
                    const int index  = (row + column) * 4;

                    atlas.image[index]     = 0;
                    atlas.image[index + 1] = 0;
                    atlas.image[index + 2] = 0;
                    atlas.image[index + 3] = static_cast<unsigned char*>(
                        glyph->bitmap.buffer)[j * glyph->bitmap.width + k];
                }
            }
            x += glyph->bitmap.width + offset_;
        }
        return atlas;
    }

    Font::Font(const SimpleString& file, const SimpleString& name)
    {
        static bool freetypeInitialized = false;

        if(!freetypeInitialized)
        {
            freetype_library = new FT_Library();
            FT_Init_FreeType(freetype_library);
            freetypeInitialized = true;
        }

        // TODO : need to put somewhere
        // FT_Done_FreeType(*freetype_library);

        log_info(("Constructing font : " + file).c_str());

        std::ifstream font_file(file.c_str(), std::ifstream::binary);

        if(!font_file.is_open())
            throw std::invalid_argument(
                ("File at \"" + file + "\" could not be opened").c_str());

        while(!font_file.eof())
        {
            auto& configuration =
                *configurations_.emplace_back(make_unique<Configuration>());
            long width;
            long h;

            font_file.read(reinterpret_cast<char*>(&configuration.size),
                           sizeof configuration.size);

            font_file.read(reinterpret_cast<char*>(&configuration.rendering_mode),
                           sizeof configuration.rendering_mode);

            font_file.read(reinterpret_cast<char*>(&configuration.ascent),
                           sizeof configuration.ascent);

            font_file.read(reinterpret_cast<char*>(&configuration.descent),
                           sizeof configuration.descent);

            font_file.read(reinterpret_cast<char*>(&configuration.height),
                           sizeof configuration.height);

            font_file.read(reinterpret_cast<char*>(&width), sizeof width);

            font_file.read(reinterpret_cast<char*>(&h), sizeof h);

            auto* pixels = new unsigned char[width * h * 4];

            font_file.read(reinterpret_cast<char*>(pixels), width * h * 4);

            size_t charactersSize = 0;

            font_file.read(reinterpret_cast<char*>(&charactersSize),
                           sizeof charactersSize);

            for(int i = 0; i < charactersSize; i++)
            {
                CharacterInfo characterInfo;
                font_file.read(reinterpret_cast<char*>(&characterInfo),
                               sizeof(CharacterInfo));
                configuration.characters.emplace(characterInfo.charCode, characterInfo);
            }

            static std::map<RenderingMode, SimpleString> conversion {
                {RenderingMode::Normal, "normal"},
                {RenderingMode::Light, "light"},
                {RenderingMode::Mono, "mono"},
                {RenderingMode::LCD, "lcd"},
                {RenderingMode::LCD_V, "lcd_v"}};

            auto* face = new FT_Face();

            auto p = file.substr(0, file.size() - 3);
            p += "ttf";

            const auto error = FT_New_Face(*freetype_library, p.c_str(), 0, face);

            FT_Set_Pixel_Sizes(*face, 0, configuration.size);

            //auto atlas = build_atlas(configuration, face);

            hb_font_t* hb_font;
            hb_font = hb_ft_font_create(*face, NULL);

            configuration.hb_font = hb_font;

            /* Create hb-buffer and populate. */
            hb_buffer_t* hb_buffer;
            hb_buffer = hb_buffer_create();
            hb_buffer_add_utf8(hb_buffer, "FILTERS", -1, 0, -1);
            hb_buffer_guess_segment_properties(hb_buffer);

            /* Shape it! */
            hb_shape(hb_font, hb_buffer, NULL, 0);

            /* Get glyph information and positions out of the buffer. */

            unsigned int         len  = hb_buffer_get_length(hb_buffer);
            hb_glyph_info_t*     info = hb_buffer_get_glyph_infos(hb_buffer, NULL);
            hb_glyph_position_t* pos  = hb_buffer_get_glyph_positions(hb_buffer, NULL);

            /* Print them out as is. */
            //printf("Raw buffer contents:\n");
            for(unsigned int i = 0; i < len; i++)
            {
                hb_codepoint_t gid       = info[i].codepoint;
                unsigned int   cluster   = info[i].cluster;
                double         x_advance = pos[i].x_advance / 64.;
                double         y_advance = pos[i].y_advance / 64.;
                double         x_offset  = pos[i].x_offset / 64.;
                double         y_offset  = pos[i].y_offset / 64.;

                char glyphname[32];
                hb_font_get_glyph_name(hb_font, gid, glyphname, sizeof(glyphname));

                printf("glyph='%s'	cluster=%d	advance=(%g,%g)	offset=(%g,%g)\n",
                       glyphname, cluster, x_advance, y_advance, x_offset, y_offset);
            }

            // printf("Converted to absolute positions:\n");

            std::vector<float> hbPos;

            /* And converted to absolute positions. */
            {
                double current_x = 0;
                double current_y = 0;
                for(unsigned int i = 0; i < len; i++)
                {
                    hb_codepoint_t gid        = info[i].codepoint;
                    unsigned int   cluster    = info[i].cluster;
                    double         x_position = current_x + pos[i].x_offset / 64.;
                    double         y_position = current_y + pos[i].y_offset / 64.;

                    hbPos.push_back(x_position);

                    char glyphname[32];
                    hb_font_get_glyph_name(hb_font, gid, glyphname, sizeof(glyphname));

                    printf("glyph='%s'	cluster=%d	position=(%g,%g)\n", glyphname,
                           cluster, x_position, y_position);

                    current_x += pos[i].x_advance / 64.;
                    current_y += pos[i].y_advance / 64.;
                }
            }

            //std::vector<unsigned char> flipped_image(atlas.width * atlas.height * 4);

            /*for(int i = 0; i < atlas.height; i++)
                for(int j = 0; j < atlas.width; j++)
                {
                    flipped_image[(i * atlas.width + j) * 4] =
                        atlas.image[((atlas.height - 1 - i) * atlas.width + j) * 4];
                    flipped_image[(i * atlas.width + j) * 4 + 1] =
                        atlas.image[((atlas.height - 1 - i) * atlas.width + j) * 4 + 1];
                    flipped_image[(i * atlas.width + j) * 4 + 2] =
                        atlas.image[((atlas.height - 1 - i) * atlas.width + j) * 4 + 2];
                    flipped_image[(i * atlas.width + j) * 4 + 3] =
                        atlas.image[((atlas.height - 1 - i) * atlas.width + j) * 4 + 3];
                }*/

            auto n = (name + "-size:" + to_string(configuration.size) + "-" +
                      conversion[configuration.rendering_mode]);

            configuration.texture.reset(new Texture(
                n.c_str(), width, h, Texture::MinFilter::Nearest,
                Texture::MagFilter::Nearest, Texture::Wrap::Repeat, Texture::Wrap::Repeat,
                Texture::Format::RGBA, Texture::InternalFormat::RGBA,
                Texture::DataType::UnsignedByte, pixels));

            font_file.peek();
        }
    }
}    // namespace corgi
