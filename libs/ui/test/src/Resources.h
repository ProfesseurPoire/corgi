
#pragma once

#include <corgi/utils/ResourcesCache.h>

#include <string>
#include <vector>

namespace frog
{
    namespace Resources
    {
        namespace Textures
        {
            static corgi::Texture* grey()
            {
                return corgi::Game::instance()
                    .current_window()
                    ->resourcesCache.get<Texture>("grey.png");
            }
            static corgi::Texture* grey_pressed()
            {
                return corgi::Game::instance()
                    .current_window()
                    ->resourcesCache.get<Texture>("grey_pressed.png");
            }
            static corgi::Texture* image()
            {
                return corgi::Game::instance()
                    .current_window()
                    ->resourcesCache.get<Texture>("image.png");
            }
        }    // namespace Textures
        namespace Animations
        {
        }
    }    // namespace Resources
}    // namespace frog