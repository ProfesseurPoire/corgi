#pragma once

#include <corgi/rendering/DrawList.h>

namespace corgi
{
    class Window;

    // The idea with the window_draw_list thing was to be able to quickly draw
    // shapes on the window directly... Not really sure how to actually do this now
    // But it would be handy if I wanted to do a immediate mode GUI thingie
    // I could try to just make vbo on the fly for each shape and use the regular
    // drawing thing -> would just need to change the projection matrix before

    /*!
     * @brief   The main goal of the WindowDrawList is to adapt coordinates
     *          to the window, that's all
     */
    class WindowDrawList
    {

    public:
        WindowDrawList(const Window& window)
            : window_(window)
        {
        }

        void add_rectangle(float x, float y, float width, float height, float r, float g, float b, float a);
        void add_rectangle(float x, float y, float width, float height, const Texture& texture);
        void add_rectangle(float x, float y, const Texture& texture);

        void add_sprite(float x, float y, Sprite sprite);
        void add_sprite(float x, float y, float width, float height, Sprite sprite);

        void add_nine_slice(DrawList::NineSlice nc);
        void add(DrawList::Text text);
        void add_mesh(const Mesh& mesh, const Matrix& model_matrix, const Material& material);
        void resetStencilBuffer();

        void clear() { draw_list_.clear(); }

        DrawList& draw_list() noexcept { return draw_list_; }

    private:
        DrawList      draw_list_;
        const Window& window_;
    };
}    // namespace corgi
