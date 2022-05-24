#include <corgi/ui/Ui.h>

using namespace corgi::ui;

void Ui::enable() noexcept
{
    is_enabled_ = true;
}

void Ui::disable() noexcept
{
    is_enabled_ = false;
}

bool Ui::is_enabled() const noexcept
{
    return is_enabled_;
}

void Ui::update_events(const Inputs& events, float elapsed_time)
{
    if(!is_enabled_)
        return;

    for(auto& child : children_)
        child->update_event(events, elapsed_time);
}

void Ui::paint(Renderer& renderer)
{
    if(!is_enabled_)
        return;

    for(auto& child : children_)
        child->actual_paint(renderer);
}
