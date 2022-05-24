#include "TextUtils.h"

#include <corgi/resources/Font.h>
#include <harfbuzz/hb.h>

using namespace corgi::utils::text;

corgi::Vector<ShapedGlyph>
corgi::utils::text::buildShapedGlyphs(const char*                       text,
                                      const corgi::Font::Configuration& fontConfiguration)
{
    corgi::Vector<ShapedGlyph> shapedGlyphs;

    // We're using harfbuzz to find the glyphs needed to build the
    // correct text

    /* Create hb-buffer and populate. */
    hb_buffer_t* hb_buffer;
    hb_buffer = hb_buffer_create();
    hb_buffer_add_utf8(hb_buffer, text, -1, 0, -1);

    hb_buffer_set_direction(hb_buffer, HB_DIRECTION_LTR);

    hb_buffer_set_script(hb_buffer, HB_SCRIPT_LATIN);
    hb_buffer_set_language(hb_buffer, hb_language_from_string("en", -1));
    hb_buffer_set_cluster_level(hb_buffer, HB_BUFFER_CLUSTER_LEVEL_MONOTONE_CHARACTERS);

    auto hb_font = static_cast<hb_font_t*>(fontConfiguration.hb_font);

    // The core HarfBuzz function that actually shape the given text
    hb_shape(hb_font, hb_buffer, NULL, 0);

    unsigned int         len  = hb_buffer_get_length(hb_buffer);
    hb_glyph_info_t*     info = hb_buffer_get_glyph_infos(hb_buffer, NULL);
    hb_glyph_position_t* pos  = hb_buffer_get_glyph_positions(hb_buffer, NULL);

    for(unsigned int i = 0; i < len; i++)
    {
        ShapedGlyph shapedGlyph;
        shapedGlyph.advance.x = static_cast<double>(pos[i].x_advance) / 64.;
        shapedGlyph.offset.x  = static_cast<double>(pos[i].x_offset) / 64.;
        shapedGlyph.offset.y  = static_cast<double>(pos[i].y_offset) / 64.;
        shapedGlyph.advance.y = static_cast<double>(pos[i].y_advance) / 64.;
        shapedGlyph.codepoint = info[i].codepoint;
        shapedGlyph.cluster   = info[i].cluster;
        shapedGlyphs.push_back(shapedGlyph);
    }

    int i = 0;
    int j = 0;

    float offsetY        = 0.0f;
    int   characterIndex = 0;

    int textSize = strlen(text);

    for(int i = 0; i < shapedGlyphs.size(); i++)
    {
        auto& glyph = shapedGlyphs[i];

        // In case there is another glyph after the one we're currentyly
        // iterating on
        if(i + 1 < shapedGlyphs.size())
        {
            auto& nextGlyph = shapedGlyphs[i + 1];

            // Now we iterator over the difference between the 2 clusters

            for(int j = glyph.cluster; j < nextGlyph.cluster; j++)
            {
                glyph.characters.push_back(text[characterIndex++]);
                glyph.textOffset = characterIndex;
            }
        }
        else
        {
            // Otherwise we just go until we processed every characters
            for(int j = characterIndex; j < textSize; j++)
            {
                glyph.characters.push_back(text[j]);
                glyph.textOffset = j;
            }
        }
    }

    hb_buffer_destroy(hb_buffer);

    return shapedGlyphs;
}

// corgi::Vector<Glyph> findGlyphs(const char* text)
// {

//     // auto lh = static_cast<float>(
//     //     font.font_->configurations_[font.current_configuration_]->height);
//     // auto a = static_cast<float>(
//     //     font.font_->configurations_[font.current_configuration_]->ascent);
//     // auto descent = static_cast<float>(
//     //     font.font_->configurations_[font.current_configuration_]->descent);

//     // auto hb_font = font.font_->configurations_[font.current_configuration_]->hb_font;

//     // // We're using harfbuzz to find the glyphs needed to build the
//     // // correct text

//     // /* Create hb-buffer and populate. */
//     // hb_buffer_t* hb_buffer;
//     // hb_buffer = hb_buffer_create();
//     // hb_buffer_add_utf8(hb_buffer, text, -1, 0, -1);

//     // hb_shape(hb_font, hb_buffer, NULL, 0);
//}
