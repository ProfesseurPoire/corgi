#pragma once

#include <corgi/ui/Widget.h>

#include <memory>
#include <vector>

namespace corgi::ui
{
    class Ui : public Widget
    {
    public:
        using Children = std::vector<std::unique_ptr<Widget>>;

        // Accessors

        [[nodiscard]] bool      is_enabled() const noexcept;
        [[nodiscard]] bool      is_world_space() const { return is_world_space_; }
        [[nodiscard]] Children& children() { return children_; }

        // Functions

        void enable() noexcept;
        void disable() noexcept;

        void is_world_space(bool v) { is_world_space_ = v; }

        /*!
    	 * @brief	Draws the Ui's widgets
    	 *
    	 *			What it actually does is send temporary primitives to the
    	 *			renderer's window_draw_list 
    	 */
        void paint(Renderer& renderer);
        void update_events(const Inputs& events, float elapsed_time);

        template<class T, class... Args>
        T& emplace_back(Args&&... args)
        {
            auto& up = children_.emplace_back(new T(nullptr, std::forward<Args>(args)...));
            return *dynamic_cast<T*>(up.get());
        }

    private:
        bool     is_enabled_ {true};
        bool     is_world_space_ {false};
        Children children_;
    };
}    // namespace corgi::ui