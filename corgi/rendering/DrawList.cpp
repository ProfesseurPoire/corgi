#include <corgi/logger/log.h>
#include <corgi/math/MathUtils.h>
#include <corgi/rendering/DrawList.h>
#include <corgi/rendering/Material.h>
#include <corgi/rendering/texture.h>
#include <corgi/resources/Font.h>
#include <corgi/utils/ResourcesCache.h>
#include <harfbuzz/hb.h>

using namespace corgi;

void DrawList::add_nine_slice(NineSlice nine_slice)
{
    order_.push_back({DrawListType::NineSclice, nine_slices_.size()});
    nine_slices_.push_back(nine_slice);
}

void DrawList::add_mesh(const Mesh&     mesh,
                        const Matrix&   model_matrix,
                        const Material& material)
{
    order_.push_back({DrawListType::Mesh, _meshes.size()});
    _meshes.emplace_back(MeshToRender {mesh, material, model_matrix});
}

void DrawList::resetStencilBuffer()
{
    order_.push_back({DrawListType::ResetStencilBuffer, resetStencilBuffer_.size()});
    // We don't really car what we push for now, we just need to know we need
    // to reset the stencil buffer at one point
    resetStencilBuffer_.push_back('c');
}

void DrawList::add_sprite(DrawListSprite sprite)
{
    order_.push_back({DrawListType::Sprite, sprites_.size()});
    sprites_.push_back(sprite);
}

void DrawList::add_rectangle(Rectangle rectangle)
{
    order_.push_back({DrawListType::Rectangle, rectangles_.size()});
    rectangles_.push_back(rectangle);
}

void DrawList::add(Text text)
{
    order_.push_back({DrawListType::Text, texts_.size()});
    texts_.push_back(text);
}

void DrawList::add_rectangle(
    float x, float y, float width, float height, float r, float g, float b, float a)
{
    order_.push_back({DrawListType::Rectangle, rectangles_.size()});
    auto material =
        ResourcesCache::get<Material>("corgi/materials/unlit/unlit_color.mat");

    if(!material)
    {
        log_fatal_error("Could not find unlit_color material");
    }

    material->set_uniform("main_color", Vec4(r, g, b, a));
    rectangles_.emplace_back(x, y, width, height, *material);
}

void DrawList::add_rectangle(
    float x, float y, float width, float height, const Texture& texture)
{
    order_.push_back({DrawListType::Rectangle, rectangles_.size()});
    auto material =
        ResourcesCache::get<Material>("corgi/materials/unlit/unlit_texture.mat");

    material->add_texture(texture);
    rectangles_.emplace_back(x, y, width, height, *material);
}

void DrawList::add_line(float x, float y, float x2, float y2)
{
    order_.push_back({DrawListType::Line, lines_.size()});
    lines_.push_back({x, y, 0.0f, x2, y2, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f});
}

void DrawList::add_line(
    float x, float y, float x2, float y2, float r, float g, float b, float a)
{
    order_.push_back({DrawListType::Line, lines_.size()});
    lines_.push_back({x, y, 0.0f, x2, y2, 0.0f, r, g, b, a});
}

void DrawList::add_line(Ray ray)
{
    order_.push_back({DrawListType::Line, lines_.size()});
    add_line(ray, 1.0f, 1.0f, 1.0f, 1.0f);
}

void DrawList::add_line(Ray ray, float r, float g, float b, float a)
{
    order_.push_back({DrawListType::Line, lines_.size()});
    lines_.push_back({ray.start.x, ray.start.y, ray.start.z,

                      ray.start.x + (ray.direction.x * ray.length),
                      ray.start.y + (ray.direction.y * ray.length),
                      ray.start.z + (ray.direction.z * ray.length),

                      r, g, b, a});
}

void DrawList::add_line(float x, float y, float z, float x2, float y2, float z2)
{
    order_.push_back({DrawListType::Line, lines_.size()});
    lines_.push_back({x, y, z, x2, y2, z2, 1.0f, 1.0f, 1.0f, 1.0f});
}

void DrawList::add_line(float x,
                        float y,
                        float z,
                        float x2,
                        float y2,
                        float z2,
                        float r,
                        float g,
                        float b,
                        float a)
{
    order_.push_back({DrawListType::Line, lines_.size()});
    lines_.push_back({x, y, z, x2, y2, z2, r, g, b, a});
}

void DrawList::Text::setText(
    const corgi::Vector<corgi::utils::text::ShapedGlyph>& shapedGlyph)
{
    shapedGlyphs_ = shapedGlyph;
    update();
}

void DrawList::Text::set_text(const SimpleString& text, bool force)
{
    if(force)
    {
        this->text = text;
        update();
        return;
    }

    if(text != this->text)
    {
        this->text = text;
        update();
    }
}

Vector<int> DrawList::Text::charactersWidth(const SimpleString& text) const
{
    Vector<int> widths;

    for(auto shapedGlyph : shapedGlyphs_)
    {
        widths.push_back(shapedGlyph.advance.x);
    }

    return widths;
}

float DrawList::Text::textWidth(const SimpleString& text) const
{
    double width = 0.0;

    for(auto shapedGlyph : shapedGlyphs_)
    {
        width += shapedGlyph.advance.x;
    }

    return width;
}

void DrawList::Text::update()
{
    // This variable mostly use in case I want an easy way to flip the v from uv coordinates
    // because of the fact that screen UI elements are inverted on the y axis
    bool flipY = false;

    if(font.font_ == nullptr)
        log_fatal_error("Font for DrawList::Text is null");

    line_height = static_cast<float>(font.height());

    ascent = static_cast<float>(font.ascent());

    auto descent = static_cast<float>(font.descent());

    double real_advance_x = 0.0;

    text_real_width = 0;

    lines_width.clear();
    std::vector<int> lines_characters;

    // Here we're simply initializing the mesh data
    std::vector<float>    vertices(5 * 4 * (shapedGlyphs_.size()), 0.0f);
    std::vector<unsigned> indexes(shapedGlyphs_.size() * 6);

    // Preparing the quads indexes
    for(int i = 0; i < shapedGlyphs_.size(); i++)
    {
        indexes[i * 6 + 0] = (i * 4) + 0;
        indexes[i * 6 + 1] = (i * 4) + 1;
        indexes[i * 6 + 2] = (i * 4) + 2;

        indexes[i * 6 + 3] = (i * 4) + 0;
        indexes[i * 6 + 4] = (i * 4) + 2;
        indexes[i * 6 + 5] = (i * 4) + 3;
    }

    Vec3 offset(position.x, position.y);
    // Selected the closest configuration according to the ViewFont parameter

    auto& conf = *font.font_->configurations_[font.current_configuration_index_];

    int line_count = 1;
    int characters = 0;

    float total_height = 0.0f;

    float text_max_height = 0.0f;
    float text_min_height = 0.0f;
    float baseline        = 0.0f;

    offset.y += ascent;

    // We need to create a quad to display each glyph
    for(int i = 0; i < shapedGlyphs_.size(); i++)
    {
        // Adds the new character
        characters++;

        // codepoint 0 seems to be equals to \n
        if(shapedGlyphs_[i].codepoint == 0)
        {
            // Adding a line
            line_count++;

            // We save the lines width just to be able to actual width of the text item
            lines_width.push_back(text_real_width);

            // We save the glyphs on 1 line
            lines_characters.push_back(characters);
            characters = 0;

            total_height += line_height;

            if(flipY)
            {
                offset.y -= line_height;
                baseline -= line_height;
            }
            else
            {
                offset.y += line_height;
                baseline += line_height;
            }
            text_real_width = 0;
            real_advance_x  = 0.0;
            continue;
        }

        // Maybe I just skip if codepoint equals 0

        GlyphInfo ci = conf.glyphs.at(shapedGlyphs_[i].codepoint);

        const auto texture_width  = static_cast<float>(conf.texture->width());
        const auto texture_height = static_cast<float>(conf.texture->height());

        if(i == 0)
        {
            if(text_max_height < ci.bitmap_top)
            {
                text_max_height = ci.bitmap_top;
            }
        }

        float vvv = baseline - (ci.glyph_height - ci.bitmap_top);

        if(text_min_height > vvv)
            text_min_height = vvv;

        float v  = (texture_height - ci.glyph_height) / texture_height;
        float aa = ci.glyph_width / texture_width;

        const float glyph_x_offset = ci.bearing_x;
        const float glpyh_y_offset =
            -(ci.glyph_height * scaling - ci.bitmap_top * scaling);
        const float glyph_width  = ci.glyph_width * scaling;
        const float glyph_height = ci.glyph_height * scaling;    // abusing a bit here

        int ii = i * 20;

        //std::cout << "OffsetX " << offset.x << std::endl;
        std::cout << "text_real_width " << text_real_width << std::endl;
        std::cout << "real_advance_x " << real_advance_x << std::endl;
        //std::cout << "glyph_x_offset " << glyph_x_offset << std::endl;
        std::cout << "glyph_width " << glyph_width << std::endl;
        //std::cout << "ax " << ci.ax << std::endl;
        std::cout << "glyph " << shapedGlyphs_[i].characters.front() << std::endl;

        // TODO : Check to make this works both in world space and screen space.
        // Probably juste have to change the sign of y depending on which space we are
        vertices[ii + 0] = offset.x + text_real_width + glyph_x_offset;
        vertices[ii + 1] = offset.y - glpyh_y_offset;
        vertices[ii + 2] = offset.z;
        vertices[ii + 3] = ci.tx;
        vertices[ii + 4] = v;

        vertices[ii + 5] = offset.x + text_real_width + glyph_x_offset + glyph_width;
        vertices[ii + 6] = offset.y - glpyh_y_offset;
        vertices[ii + 7] = offset.z;
        vertices[ii + 8] = ci.tx + aa;
        vertices[ii + 9] = v;

        vertices[ii + 10] = offset.x + text_real_width + glyph_x_offset + glyph_width;
        vertices[ii + 11] = offset.y - glpyh_y_offset - glyph_height;
        vertices[ii + 12] = offset.z;
        vertices[ii + 13] = ci.tx + aa;
        vertices[ii + 14] = 1.0f;

        vertices[ii + 15] = offset.x + text_real_width + glyph_x_offset;
        vertices[ii + 16] = offset.y - glpyh_y_offset - glyph_height;
        vertices[ii + 17] = offset.z;
        vertices[ii + 18] = ci.tx;
        vertices[ii + 19] = 1.0f;

        real_advance_x += shapedGlyphs_[i].advance.x;

        // Todo : I'm not exactly sure what's the best way to handle this here
        // because I usually have non integer value, which means : problems
        text_real_width = math::ceil(real_advance_x);
    }

    total_height = text_max_height - text_min_height;

    lines_width.push_back(text_real_width);
    lines_characters.push_back(characters);

    //float horizontal_alignment_offset = 0.0f;

    int current_line      = 0;
    int current_character = 0;

    for(auto line_width : lines_width)
    {
        switch(horizontal_alignment)
        {
            case DrawList::Text::HorizontalAlignment::Left:
                break;

            case DrawList::Text::HorizontalAlignment::Right:

                for(int i = current_character;
                    i < current_character + lines_characters[current_line]; ++i)
                {
                    vertices[i * 5 * 4] += (int)(dimensions.x - line_width);
                    vertices[i * 5 * 4 + 5] += (int)(dimensions.x - line_width);
                    vertices[i * 5 * 4 + 10] += (int)(dimensions.x - line_width);
                    vertices[i * 5 * 4 + 15] += (int)(dimensions.x - line_width);
                }

                break;

            case DrawList::Text::HorizontalAlignment::Centered:

                for(int i = current_character;
                    i < current_character + lines_characters[current_line]; ++i)
                {
                    vertices[i * 5 * 4] += (int)(dimensions.x / 2.0f - line_width / 2.0f);
                    vertices[i * 5 * 4 + 5] +=
                        (int)(dimensions.x / 2.0f - line_width / 2.0f);
                    vertices[i * 5 * 4 + 10] +=
                        (int)(dimensions.x / 2.0f - line_width / 2.0f);
                    vertices[i * 5 * 4 + 15] +=
                        (int)(dimensions.x / 2.0f - line_width / 2.0f);
                }
                break;
        }

        current_character += lines_characters[current_line];
        current_line++;
    }

    current_line      = 0;
    current_character = 0;

    for(int i = 0; i < lines_width.size(); ++i)
    {
        switch(vertical_alignment)
        {
            case DrawList::Text::VerticalAlignment::Top:
                for(int i = current_character;
                    i < current_character + lines_characters[current_line]; ++i)
                {
                    vertices[i * 5 * 4 + 1] -= (int)(ascent * scaling);
                    vertices[i * 5 * 4 + 6] -= (int)(ascent * scaling);
                    vertices[i * 5 * 4 + 11] -= (int)(ascent * scaling);
                    vertices[i * 5 * 4 + 16] -= (int)(ascent * scaling);
                }
                break;

            case DrawList::Text::VerticalAlignment::Down:

                for(int i = current_character;
                    i < current_character + lines_characters[current_line]; ++i)
                {
                    float val = -text_max_height * scaling - dimensions.y;
                    val += total_height * scaling;

                    vertices[i * 5 * 4 + 1] += val;
                    vertices[i * 5 * 4 + 6] += val;
                    vertices[i * 5 * 4 + 11] += val;
                    vertices[i * 5 * 4 + 16] += val;
                }
                break;

            case DrawList::Text::VerticalAlignment::Centered:
                for(int i = current_character;
                    i < current_character + lines_characters[current_line]; ++i)
                {
                    // I'm clearly not 100% sure about this, but the result seems decent enough.
                    float h = line_count * line_height;

                    auto val = (dimensions.y / 2.0f) - (h / 2.0f);
                    val      = int(val);

                    vertices[i * 5 * 4 + 1] += val;
                    vertices[i * 5 * 4 + 6] += val;
                    vertices[i * 5 * 4 + 11] += val;
                    vertices[i * 5 * 4 + 16] += val;
                }
                break;
        }

        current_character += lines_characters[current_line];
        current_line++;
    }
    mesh = Mesh::new_standard_2D_mesh(std::move(vertices), std::move(indexes));
}

void DrawList::clear()
{
    rectangles_.clear();
    lines_.clear();
    nine_slices_.clear();
    texts_.clear();
    order_.clear();
    _meshes.clear();
}
