#pragma once

#include "DrawList.h"

#include <corgi/math/Matrix.h>
#include <corgi/math/Ray.h>
#include <corgi/math/Vec2.h>

#include <corgi/rendering/Material.h>
#include <corgi/rendering/Sprite.h>

#include <corgi/resources/FontView.h>
#include <corgi/resources/Mesh.h>

#include <corgi/utils/TextUtils.h>

#include <vector>
#include <string>
#include <memory>

namespace corgi
{
    class Mesh;
    class Font;
    class Window;

    // So for now this kinda work, but obviously it needs to be expanded, and it's
    // very not performant at all, but if this is just to display a few boxes
    // for GUI stuff I think it should be all right. Alternatively I could also
    // do it through a "true" mesh etc but well, I'll see
    class DrawList
    {
    public:
        friend class Renderer;

        struct Color
        {
            float r, g, b, a = 1.0f;
        };

        struct Text
        {
            enum class HorizontalAlignment
            {
                Centered,
                Left,
                Right
            };

            enum class VerticalAlignment
            {
                Centered,
                Top,
                Down
            };

            std::vector<corgi::utils::text::ShapedGlyph> shapedGlyphs_;

            Vec2                position;
            Vec2                dimensions;
            std::string        text;
            Material            material;
            std::shared_ptr<Mesh> mesh;
            float               scaling = 1.0f;
            HorizontalAlignment horizontal_alignment {
                DrawList::Text::HorizontalAlignment::Centered};
            VerticalAlignment vertical_alignment {
                DrawList::Text::VerticalAlignment::Centered};
            FontView font;

            // Width of the built text
            float text_real_width;
            float line_height;
            float ascent;

            /**
             * @brief Returns the width the text will occupy
             * 
             * @param text 
             * @return float 
             */
            float textWidth(const std::string& text) const;

            std::vector<float> lines_width;

            /**
             * @brief   Returns the width of each character inside the given string
             * 
             * @param text 
             * @return Vector<float> 
             */
            Vector<int> charactersWidth(const std::string& text) const;

            void set_text(const std::string& text, bool force = false);

            void setText(const std::vector<corgi::utils::text::ShapedGlyph>& glyphs);

            void update();
        };

        struct Rectangle
        {
            Rectangle() = default;

            Rectangle(
                float    p_x,
                float    p_y,
                float    p_width,
                float    p_height,
                Material p_material
                )
                : x(p_x)
                  , y(p_y)
                  , width(p_width)
                  , height(p_height)
                  , material(p_material)
            { }

            float    x,     y;
            float    width, height;
            Material material;
        };

        struct NineSlice
        {
            float x {0.0f};
            float y {0.0f};
            float width {0.0f};
            float height {0.0f};

            float top_border_ {0.0f};
            float bottom_border_ {0.0f};
            float left_border_ {0.0f};
            float right_border_ {0.0f};

            float top_border_uv_ {0.0f};
            float bottom_border_uv_ {0.0f};
            float left_border_uv_ {0.0f};
            float right_border_uv_ {0.0f};

            Material material;

            void position(float xx, float yy)
            {
                x = xx;
                y = yy;
            }

            void dimensions(float w, float h)
            {
                width  = w;
                height = h;
            }

            void rect(float xx, float yy, float w, float h)
            {
                x      = xx;
                y      = yy;
                width  = w;
                height = h;
            }
        };

        struct Line
        {
            float x,  y,  z;
            float x2, y2, z2;
            float r,  g,  b, a;
        };

        struct Circle
        {
            float x {0.0f};
            float y {0.0f};
            float z {0.0f};
            float radius {0.0f};
            float discretisation {0.0f};
            Color color;
        };

        struct DrawListSprite
        {
            float    x {0.0f};
            float    y {0.0f};
            float    width {0.0f};
            float    height {0.0f};
            Sprite   sprite;
            Material material;
        };

        Vector<std::shared_ptr<Mesh>> meshes;

        void clear();

        void add(Text text);

        void add_nine_slice(NineSlice nine_slice);
        void add_mesh(const Mesh&     mesh,
                      const Matrix&   model_matrix,
                      const Material& material,
                      Window*         window = nullptr
            );

        void add_sprite(DrawListSprite sprite);

        void add_rectangle(Rectangle rectangle);
        void add_rectangle(float x,
                           float y,
                           float width,
                           float height,
                           float r = 1.0f,
                           float g = 1.0f,
                           float b = 1.0f,
                           float a = 1.0f
            );
        void add_rectangle(
            float          x,
            float          y,
            float          width,
            float          height,
            const Texture& texture
            );

        void add_line(float x, float y, float x2, float y2);
        void add_line(float x, float y, float z, float x2, float y2, float z2);

        void add_line(
            float x,
            float y,
            float x2,
            float y2,
            float r,
            float g,
            float b,
            float a
            );
        void add_line(float x,
                      float y,
                      float z,
                      float x2,
                      float y2,
                      float z2,
                      float r,
                      float g,
                      float b,
                      float a
            );

        void add_line(Ray ray);
        void add_line(Ray ray, float r, float g, float b, float a);

        /**
     * @brief Adds a command that will reset the StencilBuffer
     * 
     */
        void resetStencilBuffer();

        enum class DrawListType
        {
            Rectangle,
            Line,
            NineSclice,
            Text,
            Sprite,
            Mesh,
            ResetStencilBuffer
        };

    private:
        std::vector<Rectangle>      rectangles_;
        std::vector<Line>           lines_;
        std::vector<NineSlice>      nine_slices_;
        std::vector<Text>           texts_;
        std::vector<DrawListSprite> sprites_;
        std::vector<char>           resetStencilBuffer_;

        struct MeshToRender
        {
            const Mesh&     mesh;
            const Material& material;
            Matrix          matrix;
            corgi::Window*  window = nullptr;
        };

        std::vector<MeshToRender>            _meshes;
        std::vector<std::pair<DrawListType, int>> order_;
    };
}    // namespace corgi
