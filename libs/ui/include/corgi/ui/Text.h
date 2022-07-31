#pragma once

#include <corgi/rendering/DrawList.h>
#include <corgi/rendering/texture.h>
#include <corgi/resources/FontView.h>
#include <corgi/resources/Mesh.h>
#include <corgi/ui/TextEnums.h>
#include <corgi/ui/Widget.h>
#include <corgi/utils/TextUtils.h>

#include <memory>
#include <string>

namespace corgi::ui
{
class Text : public Widget
{
public:
    // Lifecycle

    Text(const std::string& txt = "");

    // Accessors

    /*!
         * @brief Returns the text displayed by the widget
         */
    [[nodiscard]] const std::string& text() const;

    // Mutators

    /*!
         * @brief Sets the text displayed by widget
         */
    [[deprecated("Use setText instead")]] void text(const std::string& str);

    /*!
         * @brief   Sets the displayed text
         */
    void setText(const std::string& text);

    /**
         * @brief   Set a string on a single line and fits it if needed
         * 
         *          Warning : This will only work for strings on a single line
         *          like labels and such. It'll shorten the string so it fits
         *          on a single line and then add "..." at the end of the string
         *          
         * 
         * @param text 
         * @param width 
         * @return SimpleString 
         */
    void setShortenedLineString(const std::string& text);

    void update_mesh();

    /**
         * @brief   Returns the width the given text will use 
         * 
         * @param text 
         * @return float 
         */
    [[nodiscard]] float textWidth(const std::string& text) const;

    /**
         * @brief   Sets the text's color
         * 
         * @param color 
         */
    void setColor(Color color) noexcept;

    /**
         * @brief   Returns the actual height of the text displayed
         * 
         * @return  float 
         */
    [[nodiscard]] float text_height() const;

    // Functions

    /**
         * @brief Returns how many lines in the text
         * 
         * @return int 
         */
    [[nodiscard]] int lines() const noexcept;

    /**
         * @brief       Returns the index of which line is at (x,y) position
         * 
         * @param x 
         * @param y 
         * @return int 
         */
    [[nodiscard]] int selectedLine(int x, int y) const;

    /*!
         * @brief Implement so the engine knows how to render the widget
         */
    void paint(Renderer& renderer) override;

    [[nodiscard]] corgi::HorizontalAlignment horizontal_alignment() const;
    [[nodiscard]] corgi::VerticalAlignment   vertical_alignment() const;

    /**
         * @brief Defines how the text should be aligned Horizontaly
         * 
         * @param horizontalAlignment Values are center,left or right
         */
    void setHorizontalAlignment(HorizontalAlignment horizontalAlignment);

    /**
         * @brief Defines how the text should be aligned vertically
         * 
         * @param verticalAlignment Values are Center, Top, Down 
         */
    void setVerticalAlignment(VerticalAlignment verticalAlignment);

    /**
         * @brief   Returns the width of the actual text mesh object
         */
    [[nodiscard]] float getRealWidth() const;

    /**
         * @brief Returns the left offset depending on the text's alignment
         * 
         * @return float 
         */
    [[nodiscard]] float leftOffset() const noexcept;

    /**
         * @brief   Returns the distance between the widget's left side and the line actual left
         * 
         *          Depends on the horizontal alignmnent
         * 
         * @param line 
         * @return float 
         */
    [[nodiscard]] float distanceLeft(int line);

    /**
         * @brief   Returns the distance between the widget's top and the actual text's top
         * 
         *          The value returned by this function depends on the Vertical Alignment
         *          used and the text's height
         * 
         * @return float 
         */
    [[nodiscard]] float text_top_distance() const noexcept;

    [[nodiscard]] const std::vector<corgi::utils::text::ShapedGlyph>&
    shapedGlyphs() const noexcept;

    /**
         * @brief Set size of the font, if available
         *        This function will simply loop through every configuration
         *        available looking for on with the requested size
         * 
         * 
         * @param size 
         */
    void setFontSize(int size);

    FontView font_view;

    void setFont(const corgi::Font& font);
    void setFont(const corgi::Font* font);

    void setDepth(float depth);

private:
    // Functions

    // Variables

    DrawList::Text _text;
    // In case the string is simplified, we still have the
    // actual string in there
    std::string actualText_;

    std::vector<corgi::utils::text::ShapedGlyph> shapedGlyphs_;

    Color mColor {0, 0, 0, 255};

    VerticalAlignment     vertical_alignment_   = VerticalAlignment::Center;
    HorizontalAlignment   horizontal_alignment_ = HorizontalAlignment::Center;
    float                 depth_                = 0.0f;
    std::shared_ptr<Mesh> _mesh;
};
}    // namespace corgi::ui
