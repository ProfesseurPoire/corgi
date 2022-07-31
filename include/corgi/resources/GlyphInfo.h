#pragma once

namespace corgi
{
    /**
     * @brief Stores a glyph information
     */
    struct GlyphInfo
    {
        /**
         * @brief Index of the character in the font
         */
        int glyph_index;

        /**
         * @brief Distance in x position to increment to reposition the pen
         */
        float advance_x {0.0f};

        /**
         * @brief Distance in y position to increment to reposition the pen
         */
        float advance_y {0.0f};

        /**
         * @brief Width of the glyph image
         */
        float glyph_width {0.0f};

        /**
         * @brief Height of the glyph image
         */
        float glyph_height {0.0f};    // bitmap width and row

        /**
         * @brief Distance between the pen position and the glyph on the x axis
         * 
         *        See https://freetype.org/freetype2/docs/tutorial/step2.html
         *                __ _ 
         *       |       / _` |
         *       |      | (_| |
         *       |       \__, |
         *       *-------|___/ 
         *       p   bx 
         * 
         *       p  : pen position 
         *       bx : bearing_x
         */
        float bearing_x {0.0f};

        /**
         * @brief   Distance between the pen position and the glypht top on the y axis
         *        
         *          See https://freetype.org/freetype2/docs/tutorial/step2.html
         *                __ _ 
         *       |       / _` |
         *     by|      | (_| |
         *       |       \__, |
         *       *-------|___/ 
         *       p    
         * 
         *       p  : pen position 
         *       by : bearing_y
         */
        float bearing_y {0.0f};

        // TODO : Check, maybe this isn't really needed anymore, and I could probably use
        //                     bearing_y instead?
        float bitmap_top {0.0f};

        /**
         * @brief x offset of glyph in texture coordinates
         */
        float tx {0.0f};
    };
};    // namespace corgi