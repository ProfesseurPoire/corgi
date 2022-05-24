#include <corgi/components/SpriteRenderer.h>
#include <corgi/main/Cursor.h>
#include <corgi/rendering/renderer.h>
#include <corgi/rendering/texture.h>
#include <corgi/resources/Font.h>
#include <corgi/ui/UiUtils.h>
#include <corgi/ui/Window.h>
#include <corgi/utils/ResourcesCache.h>

namespace corgi::ui
{
    void ui::Window::update_rectangles()
    {
        const auto offset = 6.0f;

        left_border_  = Rect(x_, y_ + offset, offset - 1.0f, height_ - offset * 2.0f - 1.0f);
        right_border_ = Rect(x_ + width_ - offset, y_ + offset, offset - 1.0f, height_ - offset * 2.0f - 1.0f);

        top_border_    = Rect(x_ + offset, y_, width_ - offset * 2.0f - 1.0f, offset - 1.0f);
        bottom_border_ = Rect(x_ + offset, y_ + height_ - offset, width_ - offset * 2 - 1.0f, offset - 1.0f);

        top_left_corner_  = Rect(x_, y_, offset - 1.0f, offset - 1.0f);
        top_right_corner_ = Rect(x_ + width_ - offset, y_, offset - 1.0f, offset - 1.0f);

        bottom_left_corner_  = Rect(x_, y_ + height_ - offset, offset - 1.0f, offset - 1.0f);
        bottom_right_corner_ = Rect(x_ + width_ - offset, y_ + height_ - offset, offset - 1.0f, offset - 1.0f);

        // TODO I set the title_bar height totally at random
        title_bar_ = Rect(x_ + offset, y_ + offset, width_ - offset * 2.0f - 1.0f, 30.0f);
    }

    ui::Window::Window()
    {
        on_mouse_over_ += [&](const int x, const int y)
        {
            const auto fx = static_cast<float>(x);
            const auto fy = static_cast<float>(y);

            corgi::Cursor::set(Cursor::DefaultCursor::Arrow);

            update_rectangles();

            if(left_border_.contains(fx, fy))
                corgi::Cursor::set(Cursor::DefaultCursor::SizeWE);

            if(right_border_.contains(fx, fy))
                corgi::Cursor::set(Cursor::DefaultCursor::SizeWE);

            if(bottom_border_.contains(fx, fy))
                corgi::Cursor::set(Cursor::DefaultCursor::SizeNS);

            if(top_border_.contains(fx, fy))
                corgi::Cursor::set(Cursor::DefaultCursor::SizeNS);

            if(bottom_left_corner_.contains(fx, fy))
                corgi::Cursor::set(Cursor::DefaultCursor::SizeNESW);

            if(bottom_right_corner_.contains(fx, fy))
                corgi::Cursor::set(Cursor::DefaultCursor::SizeNWSE);

            if(top_left_corner_.contains(fx, fy))
                corgi::Cursor::set(Cursor::DefaultCursor::SizeNWSE);

            if(top_right_corner_.contains(fx, fy))
                corgi::Cursor::set(Cursor::DefaultCursor::SizeNESW);
        };

        onMouseExit() += [&](int x, int y)
        {
            corgi::Cursor::set(Cursor::DefaultCursor::Arrow);
        };

        on_mouse_drag_begin_ += [&](const int x, const int y)
        {
            update_rectangles();

            resize_ = Resize::None;

            base_width_  = width();
            base_height_ = height();
            base_x_      = x_;
            base_y_      = y_;

            const auto fx = static_cast<float>(x);
            const auto fy = static_cast<float>(y);

            if(left_border_.contains(fx, fy))
            {
                resize_          = Resize::W;
                resize_offset_x_ = x_ - fx;
            }

            if(right_border_.contains(fx, fy))
            {
                resize_          = Resize::E;
                resize_offset_x_ = this->x() + width() - fx;
            }

            if(bottom_border_.contains(fx, fy))
            {
                resize_          = Resize::S;
                resize_offset_y_ = y_ + height() - fy;
            }

            if(top_border_.contains(fx, fy))
            {
                resize_          = Resize::N;
                resize_offset_y_ = fy - y_;
            }

            if(bottom_left_corner_.contains(fx, fy))
            {
                resize_          = Resize::SW;
                resize_offset_y_ = y_ + height() - fy;
                resize_offset_x_ = x_ - fx;
            }

            if(bottom_right_corner_.contains(fx, fy))
            {
                resize_          = Resize::SE;
                resize_offset_y_ = y_ + height() - fy;
                resize_offset_x_ = x_ + width() - fx;
            }

            if(top_left_corner_.contains(fx, fy))
            {
                resize_          = Resize::NW;
                resize_offset_y_ = fy - y_;
                resize_offset_x_ = x_ - fx;
            }

            if(top_right_corner_.contains(fx, fy))
            {
                resize_          = Resize::NE;
                resize_offset_y_ = fy - y_;
                resize_offset_x_ = this->x() + width() - fx;
            }
            move_title_bar_ = false;
            if(title_bar_.contains(fx, fy))
            {
                move_title_bar_ = true;
                off_x_          = fx - x_;
                off_y_          = fy - y_;
            }
        };

        on_mouse_drag_ += [&](const int x, const int y)
        {
            const auto fx = static_cast<float>(x);
            const auto fy = static_cast<float>(y);

            if(move_title_bar_)
            {
                this->x_ = fx - this->off_x_;
                this->y_ = fy - this->off_y_;
            }

            switch(resize_)
            {
                case Resize::E:
                    width_ = fx + resize_offset_x_ - x_;
                    break;

                case Resize::W:
                    x_     = fx + resize_offset_x_;
                    width_ = base_width_ + base_x_ - x_;
                    break;

                case Resize::N:
                    y_      = fy - resize_offset_y_;
                    height_ = base_height_ + base_y_ - y_;
                    break;

                case Resize::S:
                    height_ = fy + resize_offset_y_ - y_;
                    break;

                case Resize::NW:
                    y_      = fy - resize_offset_y_;
                    height_ = base_height_ + base_y_ - y_;
                    x_      = fx + resize_offset_x_;
                    width_  = base_width_ + base_x_ - x_;
                    break;

                case Resize::NE:
                    y_      = fy - resize_offset_y_;
                    height_ = base_height_ + base_y_ - y_;
                    width_  = fx + resize_offset_x_ - x_;
                    break;

                case Resize::SW:
                    height_ = fy + resize_offset_y_ - y_;
                    x_      = fx + resize_offset_x_;
                    width_  = base_width_ + base_x_ - x_;
                    break;

                case Resize::SE:
                    width_  = fx + resize_offset_x_ - x_;
                    height_ = fy + resize_offset_y_ - y_;
                    break;

                case Resize::None:
                    break;
            }
            // this->x_ = x - this->offset_x;
            // this->y_ = y - this->offset_y;
        };

        on_mouse_drag_end_ += [&](int x, int y) {

        };

        try
        {
            ns.material = *ResourcesCache::get<Material>("corgi/materials/ui/nine_slice.mat");
            auto* t     = ResourcesCache::get<Texture>("corgi/textures/mpb.tex");
            ns.material.add_texture(*t);
            _texture_width  = static_cast<float>(t->width());
            _texture_height = static_cast<float>(t->height());
        }
        catch(const std::exception& e)
        {
            //log_error("Could not paint ui::Window " + std::string(e.what()));
        }

        try
        {
            _text_title.material = *ResourcesCache::get<Material>("corgi/materials/unlit/unlit_texture.mat");
            font                 = ResourcesCache::get<Font>("corgi/fonts/comic_sans.fnt");
            _text_title.material.add_texture(*font->texture(), "main_texture");
        }
        catch(std::exception e)
        {
            //log_error("Can't load resource in ui::window.cpp " + std::string(e.what()));
        }
    }

    void ui::Window::paint(Renderer& renderer)
    {
        // How do I get/make the nine slice mesh?

        ns.width  = width_;
        ns.height = height_;

        ns.x = x_;
        ns.y = y_;

        //auto texture = ns.material.textures_.begin()->second;

        ns.material.enable_depth_test(false);

        ns.top_border_    = 31.0f;
        ns.bottom_border_ = 30;
        ns.left_border_   = 30.0f;
        ns.right_border_  = 30;

        /* ns.left_border_uv_   = static_cast<float>(ns.left_border_)		/	static_cast<float>(texture->width());
	    ns.right_border_uv_  = static_cast<float>(ns.right_border_)		/	static_cast<float>(texture->width());
	    ns.top_border_uv_    = static_cast<float>(ns.top_border_)		/	static_cast<float>(texture->height());
	    ns.bottom_border_uv_ = static_cast<float>(ns.bottom_border_)	/	static_cast<float>(texture->height());*/

        const float scale = 1.0f;

        ns.top_border_ *= scale;
        ns.bottom_border_ *= scale;
        ns.left_border_ *= scale;
        ns.right_border_ *= scale;

        ns.material.set_uniform("offset", x_, y_);
        ns.material.set_uniform("dimensions", width_, height_);
        ns.material.set_uniform("pivot", 0.0f, 0.0f);
        ns.material.set_uniform("borders", 30.0f, 30.0f, 30.0f, 31.0f);
        ns.material.set_uniform("texture_dimensions", _texture_width, _texture_height);

        //material->mode = Material::PolygonMode::Line;

        ns.material.enable_depth_test(false);
        ns.material.set_uniform("main_color", 1.0f, 0.0f, 0.0f, 1.0f);

        renderer.window_draw_list().add_mesh(UiUtils::nine_slice_quad(), Matrix(), ns.material);

        _text_title.scaling              = 1.0f;
        _text_title.dimensions           = Vec2(200, 30.0f);
        _text_title.horizontal_alignment = DrawList::Text::HorizontalAlignment::Left;
        _text_title.vertical_alignment   = DrawList::Text::VerticalAlignment::Top;

        _text_title.material.enable_depth_test(false);
        _text_title.font = font;

        _text_title.set_text(title_);
        renderer.window_draw_list().add_mesh(*_text_title.mesh, Matrix::translation(x_ + 3, y_, 0.0f),
                                             _text_title.material);
    }
}    // namespace corgi::ui