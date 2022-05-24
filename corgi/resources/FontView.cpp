#include <corgi/logger/log.h>
#include <corgi/resources/Font.h>
#include <corgi/resources/FontView.h>

#include <exception>
#include <stdexcept>

using namespace corgi;

void FontView::set(int size, Font::RenderingMode rendering_mode)
{
    int index = 0;

    for(const auto& configuration : font_->configurations_)
    {
        if(configuration->size == size && rendering_mode == configuration->rendering_mode)
        {
            current_configuration_index_ = index;
        }
        index++;
    }
}

void FontView::check_errors() const
{
    if(!font_)
        throw std::runtime_error("No Font associated to current FontView");

    if(font_->configurations_.empty())
        throw std::runtime_error(
            "Font associated with current FontView has no configurations");

    if(!font_->configurations_.in_range(current_configuration_index_))
        throw std::runtime_error("Current FontView configuration index is out of range");
}

int FontView::font_size() const
{
    check_errors();
    return font_->configurations_[current_configuration_index_]->size;
}

Font::RenderingMode FontView::rendering_mode() const
{
    check_errors();
    return font_->configurations_[current_configuration_index_]->rendering_mode;
}

long FontView::height() const
{
    check_errors();
    return font_->configurations_[current_configuration_index_]->height;
}

long FontView::descent() const
{
    check_errors();
    return font_->configurations_[current_configuration_index_]->descent;
}

long FontView::ascent() const
{
    check_errors();
    return font_->configurations_[current_configuration_index_]->ascent;
}