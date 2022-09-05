#include <corgi/logger/log.h>
#include <corgi/rendering/Sampler.h>
#include <corgi/rendering/texture.h>
#include <corgi/resources/Font.h>
#include <freetype2/ft2build.h>
#include <harfbuzz/hb.h>

#include FT_FREETYPE_H

#include <fstream>
#include <map>

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

const std::map<int, GlyphInfo>& Font::glyph_infos() const
{
    return configurations_[current_configuration_index_]->glyphs;
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

long long Font::memory_usage() const
{
    long long sum {0};

    long long capacity_size = static_cast<long long>(configurations_.capacity());

    for(auto& configuration : configurations_)
    {
        sum = configuration->texture->memory_usage();
    }

    return sizeof(Font) +
           capacity_size *
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

Font::Font(const std::string& file, const std::string& name)
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
            *configurations_.emplace_back(std::make_unique<Configuration>());

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

        long width;
        font_file.read(reinterpret_cast<char*>(&width), sizeof width);

        long h;
        font_file.read(reinterpret_cast<char*>(&h), sizeof h);

        auto* pixels = new unsigned char[width * h * 4];

        font_file.read(reinterpret_cast<char*>(pixels), width * h * 4);

        size_t charactersSize = 0;

        font_file.read(reinterpret_cast<char*>(&charactersSize), sizeof charactersSize);

        for(int i = 0; i < charactersSize; i++)
        {
            GlyphInfo glyph_info;
            font_file.read(reinterpret_cast<char*>(&glyph_info), sizeof(GlyphInfo));
            configuration.glyphs.emplace(glyph_info.glyph_index, glyph_info);
        }

        static std::map<RenderingMode, std::string> conversion {
            {RenderingMode::Normal, "normal"},
            {RenderingMode::Light, "light"},
            {RenderingMode::Mono, "mono"},
            {RenderingMode::LCD, "lcd"},
            {RenderingMode::LCD_V, "lcd_v"}};

        // We need the Freetype font for Harfbuzz later on
        //auto* face = new FT_Face();

        FT_Face face;

        auto p = file.substr(0, file.size() - 3);
        p += "ttf";

        const auto error = FT_New_Face(*freetype_library, p.c_str(), 0, &face);

        FT_Set_Pixel_Sizes(face, 0, configuration.size);

        // The font configuration need the hb_font later for harfbuzz shaping
        configuration.hb_font = hb_ft_font_create(face, NULL);

        auto n = (name + "-size:" + std::to_string(configuration.size) + "-" +
                  conversion[configuration.rendering_mode]);

        configuration.texture.reset(new Texture(
            n.c_str(), width, h, Texture::Format::RGBA, Texture::InternalFormat::RGBA,
            Texture::DataType::UnsignedByte, pixels));

        font_file.peek();
    }
}
}    // namespace corgi
