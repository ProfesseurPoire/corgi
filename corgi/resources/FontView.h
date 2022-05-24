#pragma once

#include <corgi/resources/Font.h>

namespace corgi
{
    class Font;

    class FontView
    {
    public:
        // Lifecycle

        FontView(const Font* font = nullptr)
            : font_(font)
        {
        }
        ~FontView()                     = default;
        FontView(const FontView& other) = default;
        FontView(FontView&& other)      = default;

        FontView& operator=(const FontView& other) = default;
        FontView& operator=(FontView&& other) = default;

        // Functions

        /**
         * @brief   Returns the line height value for the current font configuration
         * 
         * @return long 
         */
        [[nodiscard]] long height() const;

        /**
         * @brief   Returns the descent value for the current font configuration
         * 
         *          The distance from the baseline to the lowest grid 
         *          coordinate used to place an outline point
         * 
         * @return long 
         */
        [[nodiscard]] long descent() const;

        /**
         * @brief   Returns the ascent value for the current font configuration
         * 
         *          The distance from the baseline to the highest or upper grid
         *          coordinate used to place an outline point
         * @return long 
         */
        [[nodiscard]] long ascent() const;

        /**
         * @brief   Returns the font pixel size 
         * 
         * @return int 
         */
        [[nodiscard]] int font_size() const;

        [[nodiscard]] Font::RenderingMode rendering_mode() const;

        void set(int                 size,
                 Font::RenderingMode rendering_mode = Font::RenderingMode::Normal);

        const Font* font_ {nullptr};
        int         current_configuration_index_ {0};

    private:
        void check_errors() const;
    };
}    // namespace corgi
