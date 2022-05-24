#include "Game.h"

#include <corgi/inputs/Inputs.h>
#include <corgi/main/AudioPlayer.h>
#include <corgi/main/Settings.h>
#include <corgi/systems/SpriteRendererSystem.h>
#include <corgi/ui/UiUtils.h>
#include <corgi/utils/ResourcesCache.h>
#include <corgi/utils/TimeHelper.h>

namespace corgi
{
    Game::Game()
    {
        if(instance_)
            throw std::logic_error(
                "One Game object was already initialized, there can be only one");
        instance_ = this;
        settings_.initialize("resources/Settings.ini");
    }

    Game::Game(const SimpleString& project_resource_directory,
               const SimpleString& library_resource_directory)
    {
        ResourcesCache::directories().emplace_back(project_resource_directory);
        ResourcesCache::directories().emplace_back(library_resource_directory);

        if(instance_)
            throw std::logic_error(
                "1 Game object was already initialized, there can be only one");
        instance_ = this;
        settings_.initialize("resources/Settings.ini");
    }

    Game::~Game()
    {
        SpriteRendererSystem::release_sprite_mesh();
        UiUtils::release_nineslice_mesh();
    }

    float Game::time_step() { return time_step_; }

    void Game::time_step(float value) { time_step_ = value; }

    Renderer& Game::renderer() { return renderer_; }

    Settings& Game::settings() noexcept { return settings_; }

    const Inputs& Game::inputs() const noexcept { return window_.inputs_; }

    AudioPlayer& Game::audio_player() noexcept { return audio_player_; }

    void Game::initialize_window(const std::string& name,
                                 int                x,
                                 int                y,
                                 int                width,
                                 int                height,
                                 int                fullscreen,
                                 bool               vsync,
                                 bool               borderless)
    {
        window_.initialize(name.c_str(), x, y, width, height, fullscreen, vsync);
        window_.setBorderless(borderless);
        window_.show();

        renderer_.initialize();
        SpriteRendererSystem::initializeSpriteMesh();
    }

    GameScene& Game::scene() { return scene_; }

    Window& Game::window() { return window_; }

    void Game::quit() { quit_ = true; }
    void Game::run()
    {
        time_.timestep(time_step_);
        time_.start();
        profiler_.loop_counter_.start();

        std::cout << "Material Component Size : " << sizeof(Material) << std::endl;
        std::cout << "Vector Material Texture Uniform Size"
                  << sizeof(std::vector<Material::TextureUniform>) << std::endl;

        while(!window().is_closing() && !quit_)
        {
            profiler_.update();

            time_.update();
            while(time_.timestep_overrun())
            {
                profiler_.update_counter_.start();

                window_.update();
                renderer().windowDrawList().clear();
                renderer().world_draw_list().clear();
                scene().before_update(time_.timestep());

                if(update_)
                    update_();

                scene().update(time_.timestep());
                scene().after_update(time_.timestep());

                profiler_.update_counter_.tick();
            }

            // Not sure if I should pack swap_buffer with it or not
            profiler_.renderer_counter_.start();
            renderer().draw_scene();
            profiler_.renderer_counter_.tick();

            window().swap_buffers();
            profiler_.loop_counter_.tick();
        }
        // Clearing resources when exiting the main loop
        ResourcesCache::clear();
    }

    void Game::refresh_settings() { settings_.refresh(); }

    const Profiler& Game::profiler() const noexcept { return profiler_; }
}    // namespace corgi
