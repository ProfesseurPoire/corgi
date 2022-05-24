#include <corgi/logger/log.h>
#include <corgi/math/MathUtils.h>
#include <corgi/rendering/renderer.h>
#include <corgi/resources/Font.h>
#include <corgi/ui/Text.h>
#include <corgi/utils/ResourcesCache.h>

using namespace corgi;

int ui::Text::lines() const noexcept
{
    int lines = 1;

    for(auto character : actualText_)
    {
        if(character == '\n')
        {
            lines++;
        }
    }
    return lines;
}

float ui::Text::text_top_distance() const noexcept
{
    auto l = lines();

    float text_height = l * font_view.height();

    switch(vertical_alignment_)
    {
        case VerticalAlignment::Center:
            return (height() - text_height) / 2.0f;

        case VerticalAlignment::Top:
            return 0;

        case VerticalAlignment::Down:
            return height() - text_height;
    }
}

float ui::Text::text_height() const
{
    return lines() * font_view.height();
}

int ui::Text::selectedLine(int x, int y) const
{
    // We get back the actual top of the text
    auto top = real_y() + text_top_distance();
    // We get back how many lines we have
    auto l = lines();

    auto r = y - top;

    if(r >= 0 && r < text_height())
    {
        return r / font_view.height();
    }
    return -1;
}

ui::Text::Text(const SimpleString& txt)
{
    _text.scaling = 1.0f;

    _text.text = txt;

    switch(vertical_alignment_)
    {
        case VerticalAlignment::Center:
            _text.vertical_alignment = DrawList::Text::VerticalAlignment::Centered;
            break;

        case VerticalAlignment::Top:
            _text.vertical_alignment = DrawList::Text::VerticalAlignment::Top;
            break;

        case VerticalAlignment::Down:
            _text.vertical_alignment = DrawList::Text::VerticalAlignment::Down;
            break;
    }

    switch(horizontal_alignment_)
    {
        case HorizontalAlignment::Center:
            _text.horizontal_alignment = DrawList::Text::HorizontalAlignment::Centered;
            break;
        case HorizontalAlignment::Left:
            _text.horizontal_alignment = DrawList::Text::HorizontalAlignment::Left;
            break;
        case HorizontalAlignment::Right:
            _text.horizontal_alignment = DrawList::Text::HorizontalAlignment::Right;
            break;
    }

    _text.dimensions = Vec2(width_, height_);

    mMaterial = *ResourcesCache::get<Material>("corgi/materials/unlit/unlit_texture.mat");

    if(!font_view.font_)
    {
        font_view.font_ = ResourcesCache::get<Font>("corgi/fonts/Roboto-Regular.fnt");
    }

    mMaterial.enable_stencil_test(false);

    _text.font = font_view;

    auto& conf =
        *_text.font.font_->configurations_[_text.font.current_configuration_index_];

    mMaterial.set_uniform("flat_color", mColor);
    mMaterial.set_uniform("use_flat_color", 1);

    if(mMaterial._texture_uniforms.empty())
        mMaterial.add_texture(*conf.texture.get());
    else
        mMaterial.set_texture(0, *conf.texture.get());
}

float corgi::ui::Text::distanceLeft(int line)
{
    if(_text.lines_width.empty())
        return 0.0f;

    switch(horizontal_alignment_)
    {
        case HorizontalAlignment::Left:
            return 0.0f;
        case HorizontalAlignment::Center:
            return (width() - _text.lines_width[line]) / 2.0f;
        case HorizontalAlignment::Right:
            return (width() - _text.lines_width[line]);
    }
}

void corgi::ui::Text::setFont(const Font& font)
{
    font_view.font_ = &font;

    _text.font = font_view;

    auto& conf =
        *_text.font.font_->configurations_[_text.font.current_configuration_index_];

    mMaterial.set_uniform("flat_color", mColor);
    mMaterial.set_uniform("use_flat_color", 1);

    if(mMaterial._texture_uniforms.empty())
        mMaterial.add_texture(*conf.texture.get());
    else
        mMaterial.set_texture(0, *conf.texture.get());
}

void corgi::ui::Text::setFont(const Font* font)
{
    if(!font)
    {
        log_warning("Can't set nullptr font");
        return;
    }

    setFont(*font);
}

void corgi::ui::Text::setFontSize(int size)
{
    _text.font.set(size);

    auto& conf =
        *_text.font.font_->configurations_[_text.font.current_configuration_index_];

    mMaterial.set_uniform("flat_color", mColor);
    mMaterial.set_uniform("use_flat_color", 1);

    if(mMaterial._texture_uniforms.empty())
        mMaterial.add_texture(*conf.texture.get());
    else
        mMaterial.set_texture(0, *conf.texture.get());
}

const SimpleString& corgi::ui::Text::text() const
{
    return _text.text;
}

void corgi::ui::Text::text(const SimpleString& txt)
{
    auto conf =
        font_view.font_->configurations_[font_view.current_configuration_index_].get();
    shapedGlyphs_      = corgi::utils::text::buildShapedGlyphs(txt.c_str(), *conf);
    actualText_        = txt;
    _text.dimensions.x = width();
    _text.dimensions.y = height();
    _text.text         = txt;
    _text.setText(shapedGlyphs_);
}

void ui::Text::setShortenedLineString(const SimpleString& text)
{
    _text.dimensions.x = width();
    _text.dimensions.y = height();

    SimpleString textCopy = text;

    // We check the actual size of the text before hand
    auto       textWidth       = _text.textWidth(text);
    const auto charactersWidth = _text.charactersWidth(text);

    if(textWidth > width())
    {
        const auto suspensionPointWidth = _text.textWidth("...");

        int charactersRemoved = 0;

        while(textWidth + suspensionPointWidth > width())
        {
            textCopy.remove(textCopy.size() - 1);
            textWidth -=
                charactersWidth[charactersWidth.size() - 1 - charactersRemoved++];
        }
        setText(textCopy + "...");
        return;
    }
    setText(text);
}

void ui::Text::setText(const SimpleString& text)
{
    auto conf =
        font_view.font_->configurations_[font_view.current_configuration_index_].get();
    actualText_   = text;
    shapedGlyphs_ = corgi::utils::text::buildShapedGlyphs(text.c_str(), *conf);

    _text.dimensions.x = width();
    _text.dimensions.y = height();

    _text.setText(shapedGlyphs_);
}

const corgi::Vector<corgi::utils::text::ShapedGlyph>&
ui::Text::shapedGlyphs() const noexcept
{
    return shapedGlyphs_;
}

float ui::Text::leftOffset() const noexcept
{
    switch(horizontal_alignment_)
    {
        case HorizontalAlignment::Left:
            return 0.0f;

        case HorizontalAlignment::Center:
            return (width() - textWidth(actualText_)) / 2.0f;

        case HorizontalAlignment::Right:
            return (width() - textWidth(actualText_));
    }
}

float ui::Text::textWidth(const SimpleString& text) const
{
    return _text.textWidth(text);
}

void ui::Text::update_mesh()
{
    if(_text.dimensions.x != width() || _text.dimensions.y != height() ||
       _text.material != mMaterial)
    {
        _text.dimensions.x = width();
        _text.dimensions.y = height();
        _text.material     = mMaterial;

        _text.setText(shapedGlyphs_);
    }
}

void ui::Text::setColor(Color color) noexcept
{
    mColor = color;
    mMaterial.set_uniform("flat_color", mColor);
}

HorizontalAlignment ui::Text::horizontal_alignment() const
{
    return horizontal_alignment_;
}

VerticalAlignment ui::Text::vertical_alignment() const
{
    return vertical_alignment_;
}

void ui::Text::setHorizontalAlignment(HorizontalAlignment horizontalAlignment)
{
    horizontal_alignment_ = horizontalAlignment;
    _text.setText(shapedGlyphs_);
}

void ui::Text::setVerticalAlignment(VerticalAlignment verticalAlignment)
{
    vertical_alignment_ = verticalAlignment;
    _text.setText(shapedGlyphs_);
}

float ui::Text::getRealWidth() const
{
    return _text.text_real_width;
}

void corgi::ui::Text::paint(Renderer& renderer)
{
    switch(vertical_alignment_)
    {
        case VerticalAlignment::Center:
            _text.vertical_alignment = DrawList::Text::VerticalAlignment::Centered;
            break;

        case VerticalAlignment::Top:
            _text.vertical_alignment = DrawList::Text::VerticalAlignment::Top;
            break;

        case VerticalAlignment::Down:
            _text.vertical_alignment = DrawList::Text::VerticalAlignment::Down;
            break;
    }

    switch(horizontal_alignment_)
    {
        case HorizontalAlignment::Center:
            _text.horizontal_alignment = DrawList::Text::HorizontalAlignment::Centered;
            break;
        case HorizontalAlignment::Left:
            _text.horizontal_alignment = DrawList::Text::HorizontalAlignment::Left;
            break;
        case HorizontalAlignment::Right:
            _text.horizontal_alignment = DrawList::Text::HorizontalAlignment::Right;
            break;
    }

    _text.dimensions = Vec2(width(), height());

    update_mesh();
    // TODO : Using the Game Singleton to access the windows kinda sucks here
    renderer.window_draw_list().add_mesh(*_text.mesh,
                                         Matrix::translation(corgi::math::round(real_x()),
                                                             corgi::math::round(real_y()),
                                                             depth_),
                                         mMaterial);
}

void ui::Text::setDepth(float depth)
{
    depth_ = depth;
}