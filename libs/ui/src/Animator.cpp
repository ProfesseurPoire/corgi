#include <corgi/rendering/renderer.h>
#include <corgi/ui/Animator.h>

namespace corgi::ui
{
    void ui::Animator::update(const float elapsed_time)
    {
        current_time_ += elapsed_time;

        if(current_time_ > current_frame().time_)
        {
            next_frame();
            current_time_ = current_frame().time_ - current_time_;
        }
    }

    void Animator::paint(Renderer& renderer)
    {
        renderer.window_draw_list().add_sprite(real_x(), real_y(), static_cast<unsigned>(width_),
                                               static_cast<unsigned>(height_), current_frame().sprite_);
    }

    bool Animator::next_frame()
    {
        current_frame_id_++;

        if(current_frame_id_ >= static_cast<unsigned>(animation.frames.size()))
        {
            if(animation.looping)
            {
                current_frame_id_ = 0;
                return true;
            }
        }
        return false;
    }

    Animation::Frame ui::Animator::current_frame() { return animation.frames[current_frame_id_]; }
}    // namespace corgi::ui