#pragma once

#include <corgi/inputs/Inputs.h>
#include <corgi/ui/Widget.h>

namespace corgi::ui
{
    class Canvas
    {
    public:
        void updateEvents(const Inputs& inputs);
        void paint(Renderer& renderer);

    private:
        /**
         * @brief Stores the widgets that contains the mouse cursor when the 
         *        mouse down button is triggered
         */
        std::vector<Widget*> mMouseButtonDownWidgets;

        /**
         * @brief Root widget that will hold the children 
         */
        Widget mRoot;
    };
}    // namespace corgi::ui