#pragma once

#include <corgi/resources/Animation.h>
#include <corgi/ui/Widget.h>

namespace corgi::ui
{
    class Animator : public Widget
    {
    public:
        // Lifecycle

        Animator()                      = default;
        ~Animator()                     = default;
        Animator(const Animator& other) = default;
        Animator(Animator&& other)      = default;

        Animator& operator=(const Animator& other) = default;
        Animator& operator=(Animator&& other) = default;

        //  Variables

        Animation animation;

        void update(float elapsed_time) override;
        void paint(Renderer& renderer) override;

    private:
        Animation::Frame current_frame();

        /*!
		 * @brief  Tries to load the next frame into @a current_frame_
		 *
		 *@return	Returns false if we reached the last frame and if the animation doesn't loop
		 */
        bool next_frame();

        float    current_time_ {0.0f};
        unsigned current_frame_id_ {0};
    };
}    // namespace corgi::ui