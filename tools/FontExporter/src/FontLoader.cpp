#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <freetype2/ft2build.h>
#include <harfbuzz/hb.h>
#include <stdlib.h>

#include <algorithm>
#include <cstdio>
#include <exception>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <hb-ft.h>

#include FT_FREETYPE_H

//
//#ifndef STB_IMAGE_IMPLEMENTATION
//#define STB_IMAGE_IMPLEMENTATION
//#endif
//
//#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
//#define STB_IMAGE_WRITE_IMPLEMENTATION
//#endif
//
//#include <stb_image.h>
//#include <stb_image_write.h>

#include "FontLoader.h"
#include "filereadstream.h"

#include <corgi/filesystem/FileSystem.h>
#include <corgi/resources/GlyphInfo.h>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/stringbuffer.h>

static bool        freetype_initialized_ = false;
static FT_Library* freetype_library      = nullptr;

// To avoid texture filtering problems, we don't put glyphs next to each other
static const int offset_ = 10;

class FontConfiguration
{
public:
    // Enums

    enum class RenderingMode
    {
        Normal,
        Light,
        Mono,
        LCD,
        LCD_V
    };

    // Lifecycle

    FontConfiguration() = default;

    FontConfiguration(const rapidjson::Value::Object& object) { load(object); }

    ~FontConfiguration()                              = default;
    FontConfiguration(const FontConfiguration& other) = default;
    FontConfiguration(FontConfiguration&& other)      = default;

    FontConfiguration& operator=(const FontConfiguration& other) = default;
    FontConfiguration& operator=(FontConfiguration&& other)      = default;

    // Functions

    /*!
     * @brief   Saves the object inside a rapidjson serialization thingie
     */
    void save(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) const
    {
        writer.StartObject();
        writer.Key("font-size");
        writer.Int(size_);

        switch(rendering_mode_)
        {
            case RenderingMode::Normal:
                writer.Key("render_mode");
                writer.String("normal");
                break;

            case RenderingMode::Light:
                writer.Key("render_mode");
                writer.String("light");
                break;

            case RenderingMode::Mono:
                writer.Key("render_mode");
                writer.String("mono");
                break;

            case RenderingMode::LCD:
                writer.Key("render_mode");
                writer.String("lcd");
                break;

            case RenderingMode::LCD_V:
                writer.Key("render_mode");
                writer.String("lcd_v");
                break;
        }
        writer.EndObject();
    }

    void load(const rapidjson::Document::Object& conf)
    {
        if(conf.HasMember("font-size"))
        {
            size_ = conf["font-size"].GetInt();
        }

        if(conf.HasMember("render_mode"))
        {
            if(strcmp(conf["render_mode"].GetString(), "normal") == 0)
            {
                rendering_mode_ = RenderingMode::Normal;
                return;
            }

            if(strcmp(conf["render_mode"].GetString(), "light") == 0)
            {
                rendering_mode_ = RenderingMode::Light;
                return;
            }

            if(strcmp(conf["render_mode"].GetString(), "mono") == 0)
            {
                rendering_mode_ = RenderingMode::Mono;
                return;
            }

            if(strcmp(conf["render_mode"].GetString(), "lcd") == 0)
            {
                rendering_mode_ = RenderingMode::LCD;
                return;
            }

            if(strcmp(conf["render_mode"].GetString(), "lcd_v") == 0)
            {
                rendering_mode_ = RenderingMode::LCD_V;
            }
        }
    }

    [[nodiscard]] int size() const { return size_; }

    [[nodiscard]] RenderingMode rendering_mode() const { return rendering_mode_; }

private:
    int           size_ {12};
    RenderingMode rendering_mode_ {RenderingMode::Normal};
};

class FontConfigurationFile
{
public:
    // Lifecycle

    FontConfigurationFile(const std::string& path)
    {
        // If no configuration file exist, we make a default one and save it
        if(!corgi::filesystem::file_exist(path))
        {
            font_configurations_.emplace_back();
            save(path);
            return;
        }

        load(path);
    }

    void save(const std::string& path) const
    {
        std::ofstream f(path.c_str());

        if(!f.is_open())
        {
            printf("Could not make default font configuration file at %s", path.c_str());
            return;
        }

        rapidjson::StringBuffer                          sb;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
        writer.SetFormatOptions(rapidjson::kFormatDefault);
        writer.StartObject();
        writer.Key("Configurations");
        writer.StartArray();
        for(const auto& font_configuration : font_configurations_)
        {
            font_configuration.save(writer);
        }
        writer.EndArray();
        writer.EndObject();
        f << sb.GetString();
    }

    void load(const std::string& path)
    {
        if(!corgi::filesystem::file_exist(path))
        {
            throw std::invalid_argument("No file found at given path");
        }

        // We need the file's size for rapidjson::FileReadStream
        const auto size = corgi::filesystem::size(path);

#ifdef __linux__
        FILE* fp = fopen(path.c_str(), "r");
#elif _WIN32
        FILE* fp = fopen(path.c_str(), "rb");
#endif
        std::vector<char> read_buffer(static_cast<std::vector<char>::size_type>(size));

        rapidjson::FileReadStream is(fp, read_buffer.data(),
                                     static_cast<size_t>(sizeof(char) * size));
        rapidjson::Document       document;
        document.ParseStream(is);

        const auto obj = document.GetObject();

        for(auto& val : obj["Configurations"].GetArray())
            font_configurations_.emplace_back(val.GetObject());

        fclose(fp);
    }

    // Variables

    std::vector<FontConfiguration> font_configurations_;
};

bool FontLoader::initialize()
{
    freetype_library = new FT_Library();

    if(FT_Init_FreeType(freetype_library))
        throw std::runtime_error("The Freetype Library could not be initialized");

    freetype_initialized_ = true;
    return true;
}

void FontLoader::finalize()
{
    FT_Done_FreeType(*freetype_library);
}

std::pair<int, int> find_max_dimensions(FT_Face&                         face,
                                        FontConfiguration::RenderingMode renderingMode)
{
    const auto* const glyph = face->glyph;

    auto width  = 0;
    auto height = 0;

    // Only scan for the max height
    FT_UInt gindex;

    auto charCode = FT_Get_First_Char(face, &gindex);

    while(gindex != 0)
    {
        // load the glyph
        auto e = FT_Load_Glyph(face,            /* handle to face object */
                               gindex,          /* glyph index           */
                               FT_LOAD_RENDER); /* load flags, see below */

        // set it's rendering mode
        switch(renderingMode)
        {
            case FontConfiguration::RenderingMode::Normal:
                e = FT_Render_Glyph((face)->glyph,          /* glyph slot  */
                                    FT_RENDER_MODE_NORMAL); /* render mode */
                break;

            case FontConfiguration::RenderingMode::Light:
                e = FT_Render_Glyph((face)->glyph,         /* glyph slot  */
                                    FT_RENDER_MODE_LIGHT); /* render mode */
                break;

            case FontConfiguration::RenderingMode::Mono:
                e = FT_Render_Glyph((face)->glyph,        /* glyph slot  */
                                    FT_RENDER_MODE_MONO); /* render mode */
                break;

            case FontConfiguration::RenderingMode::LCD:
                e = FT_Render_Glyph((face)->glyph,       /* glyph slot  */
                                    FT_RENDER_MODE_LCD); /* render mode */
                break;

            case FontConfiguration::RenderingMode::LCD_V:
                e = FT_Render_Glyph((face)->glyph,         /* glyph slot  */
                                    FT_RENDER_MODE_LCD_V); /* render mode */
                break;
        }

        width += glyph->bitmap.width + offset_;
        height = std::max(height, static_cast<int>(glyph->bitmap.rows));

        charCode = FT_Get_Next_Char(face, charCode, &gindex);
    }
    //width -= offset_;

    return {width, height};
}

struct Atlas
{
    std::map<int, corgi::GlyphInfo> characters;
    std::vector<unsigned char>      image;
    long                            width {0};
    long                            height {0};
};

Atlas build_atlas(const FontConfiguration& configuration, FT_Face* face)
{
    Atlas atlas;

    const auto error = FT_Set_Pixel_Sizes(*face, 0, configuration.size());

    if(error != 0)
        throw std::runtime_error("Error while setting the font pixel size");

    // Btw this is probably not needed since there is something that returns the dimensions
    // already I think inside the font data
    auto [width, height] =
        find_max_dimensions(*static_cast<FT_Face*>(face), configuration.rendering_mode());

    const auto* glyph = (*static_cast<FT_Face*>(face))->glyph;

    auto x = 0;

    atlas.image.resize(width * height * 4, 0);    // using vector for raii
    atlas.width  = width;
    atlas.height = height;

    FT_UInt gindex;

    auto charCode = FT_Get_First_Char(*face, &gindex);

    int loops = 0;

    while(gindex != 0)
    {
        // load the glyph
        auto e = FT_Load_Glyph(*face,           /* handle to face object */
                               gindex,          /* glyph index           */
                               FT_LOAD_RENDER); /* load flags, see below */

        // set it's rendering mode
        switch(configuration.rendering_mode())
        {
            case FontConfiguration::RenderingMode::Normal:
                e = FT_Render_Glyph((*face)->glyph,         /* glyph slot  */
                                    FT_RENDER_MODE_NORMAL); /* render mode */
                break;

            case FontConfiguration::RenderingMode::Light:
                e = FT_Render_Glyph((*face)->glyph,        /* glyph slot  */
                                    FT_RENDER_MODE_LIGHT); /* render mode */
                break;

            case FontConfiguration::RenderingMode::Mono:
                e = FT_Render_Glyph((*face)->glyph,       /* glyph slot  */
                                    FT_RENDER_MODE_MONO); /* render mode */
                break;

            case FontConfiguration::RenderingMode::LCD:
                e = FT_Render_Glyph((*face)->glyph,      /* glyph slot  */
                                    FT_RENDER_MODE_LCD); /* render mode */
                break;

            case FontConfiguration::RenderingMode::LCD_V:
                e = FT_Render_Glyph((*face)->glyph,        /* glyph slot  */
                                    FT_RENDER_MODE_LCD_V); /* render mode */
                break;
        }

        // Build the information used to know where a character is
        // on the final image

        corgi::GlyphInfo glyph_info;
        glyph_info.glyph_index  = gindex;
        glyph_info.advance_x    = static_cast<float>(glyph->advance.x >> 6);
        glyph_info.advance_y    = static_cast<float>(glyph->advance.y >> 6);
        glyph_info.glyph_width  = static_cast<float>(glyph->bitmap.width);
        glyph_info.glyph_height = static_cast<float>(glyph->bitmap.rows);
        glyph_info.bearing_x    = static_cast<float>(glyph->metrics.horiBearingX >> 6);
        glyph_info.bitmap_top   = static_cast<float>(glyph->bitmap_top);

        glyph_info.tx = static_cast<float>(x) / static_cast<float>(width);

        atlas.characters[gindex] = glyph_info;

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

        charCode = FT_Get_Next_Char(*face, charCode, &gindex);
    }

    return atlas;
}

static void save_font(Atlas                    atlas,
                      FT_Face*                 face,
                      std::ofstream&           output_file,
                      const FontConfiguration& configuration)
{
    // Todo : Do I Really need to do this?

    std::vector<unsigned char> flipped_image(atlas.width * atlas.height * 4);

    for(int i = 0; i < atlas.height; i++)
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
        }

    // We divide by 64 because the values are in point size
    // I've seen people do >> 6 . Maybe check if it's more efficient
    // but I'm pretty sure the compiler probably optimize that itself so

    const auto ascent      = (*face)->size->metrics.ascender / 64l;
    const auto descent     = (*face)->size->metrics.descender / 64l;
    const auto line_height = (*face)->size->metrics.height / 64l;

    const auto size        = configuration.size();
    const auto render_mode = configuration.rendering_mode();

    output_file.write(reinterpret_cast<const char*>(&size), sizeof size);
    output_file.write(reinterpret_cast<const char*>(&render_mode), sizeof render_mode);
    output_file.write(reinterpret_cast<const char*>(&ascent), sizeof ascent);
    output_file.write(reinterpret_cast<const char*>(&descent), sizeof descent);
    output_file.write(reinterpret_cast<const char*>(&line_height), sizeof line_height);

    output_file.write(reinterpret_cast<char*>(&atlas.width), sizeof atlas.width);
    output_file.write(reinterpret_cast<char*>(&atlas.height), sizeof atlas.height);
    output_file.write(reinterpret_cast<char*>(flipped_image.data()),
                      flipped_image.size());

    const auto sizeCharacters = atlas.characters.size();

    output_file.write(reinterpret_cast<const char*>(&sizeCharacters), sizeof(size_t));
    for(auto pair : atlas.characters)
        output_file.write(reinterpret_cast<char*>(&pair.second),
                          sizeof(corgi::GlyphInfo));

    // DebugCode. Use it if you want to check what the font looks like

    stbi_flip_vertically_on_write(true);
    auto path = "size_" + std::to_string(configuration.size()) + "_rendering_mode_" +
                std::to_string(static_cast<int>(configuration.rendering_mode())) + ".png";
    stbi_write_png(("C:/dev/" + path).c_str(), atlas.width, atlas.height, 4,
                   flipped_image.data(), atlas.width * 4);
}

bool FontLoader::load(const std::string& path, const std::string& output_path)
{
    // TODO : Not necessary if it was an object
    // although I'd need to be sure
    if(!freetype_initialized_)
        initialize();

    //stbtt_fontinfo info;
    unsigned char* fontBuffer;

    FILE* fontFile = fopen(path.c_str(), "rb");
    fseek(fontFile, 0, SEEK_END);
    int size = ftell(fontFile);   /* how long is the file ? */
    fseek(fontFile, 0, SEEK_SET); /* reset */

    fontBuffer = new unsigned char[size];
    fread(fontBuffer, size, 1, fontFile);
    fclose(fontFile);

    // bool errorStbFont =
    //     stbtt_InitFont(&info, fontBuffer, stbtt_GetFontOffsetForIndex(fontBuffer, 0));

    //float scale = stbtt_ScaleForPixelHeight(&info, 24);

    // auto glyphLeft  = stbtt_FindGlyphIndex(&info, 'L');
    // auto glyphRight = stbtt_FindGlyphIndex(&info, 'T');

    // auto a = scale * stbtt__GetGlyphGPOSInfoAdvance(&info, glyphLeft, glyphRight);

    // auto advance = stbtt_GetGlyphKernAdvance(&info, glyphLeft, glyphRight);

    // auto kerningSize = stbtt_GetKerningTableLength(&info);

    //stbtt_kerningentry entry;

    //stbtt_GetKerningTable(&info, &entry, kerningSize);

    int ascent, descent, lineGap;
    //stbtt_GetFontVMetrics(&info, &ascent, &descent, &lineGap);

    if(!corgi::filesystem::file_exist(path))
        throw std::invalid_argument(("No file found at \"" + path + "\"").c_str());

    if(corgi::filesystem::extension(path) != "ttf")
        throw std::invalid_argument(
            ("File at \"" + path + "\" is not a .ttf file ").c_str());

    auto* face = new FT_Face();

    const auto error = FT_New_Face(*freetype_library, path.c_str(), 0, face);

    // Can't find other error code lol, can't even find where this one is defined
    if(error == FT_Err_Unknown_File_Format)
    {
        throw std::invalid_argument("Font could not be loaded : Unknown File Format");
    }
    if(error)
    {
        throw std::invalid_argument("Font could not be loaded");
    }

    std::string           conf_file_path = path.substr(0, path.size() - 4) + ".fconf";
    FontConfigurationFile font_configuration_file(conf_file_path);

    std::ifstream file(path.c_str());
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    if(!file.is_open())
        throw std::runtime_error(("Could not open file" + path).c_str());

    std::ofstream output_file(output_path.c_str(), std::ofstream::out |
                                                       std::ofstream::trunc |
                                                       std::ofstream::binary);

    if(!output_file.is_open())
        throw std::runtime_error(("Could not open file" + output_path).c_str());

    for(const auto& configuration : font_configuration_file.font_configurations_)
    {
        auto atlas = build_atlas(configuration, face);
        save_font(atlas, face, output_file, configuration);
    }
    output_file.close();
    FT_Done_Face(*face);
    return true;
}
