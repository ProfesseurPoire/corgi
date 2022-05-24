#pragma once

#include <corgi/SimpleString.h>
#include <corgi/ecs/Scene.h>
#include <corgi/inputs/Inputs.h>
#include <corgi/main/AudioPlayer.h>
#include <corgi/main/Profiler.h>
#include <corgi/main/SDLContext.h>
#include <corgi/main/Settings.h>
#include <corgi/main/Window.h>
#include <corgi/rendering/renderer.h>
#include <corgi/utils/Physic.h>
#include <corgi/utils/ResourcesCache.h>

#include <functional>

namespace corgi
{
    class ResourcesCache;
    class Window;

    /*!
	 *	Mostly here to add a few useful things, like the physic object 
	 */
    class GameScene : public Scene
    {
    public:
        Physic physic = Physic(dynamic_cast<Scene&>(*this));
    };

    /*!
	 * Contains every system needed for the game to launch. Must be initialized in the main of other project
	 */
    class Game
    {
    public:
        // Lifecycle

        Game();
        Game(const SimpleString& project_resource_directory,
             const SimpleString& library_resource_directory);
        ~Game();

        Game(const Game& other) = delete;
        Game(Game&& other)      = delete;
        Game& operator=(const Game& other) = delete;
        Game& operator=(Game&& other) = delete;

        // Accessors

        [[nodiscard]] GameScene& scene();
        [[nodiscard]] Window&    window();
        [[nodiscard]] Renderer&  renderer();
        [[nodiscard]] Settings&  settings() noexcept;

        [[nodiscard]] const Inputs& inputs() const noexcept;

        [[nodiscard]] AudioPlayer& audio_player() noexcept;

        [[nodiscard]] const Profiler& profiler() const noexcept;
        [[nodiscard]] float           time_step();

        [[nodiscard]] Time& time() { return time_; }

        /**
         * @brief Quit the game
         */
        void quit();

        // Functions

        /*!
		 * @brief Starts the game loop
		 */
        void run();

        /*!
		 * @brief Sets the fixed time between 2 logica update
		 */
        void time_step(float value);

        void initialize_window(const std::string& name,
                               int                x,
                               int                y,
                               int                width,
                               int                height,
                               int                fullscreen,
                               bool               vsync,
                               bool               borderless);

        /*!
		 * @brief Reloads the settings.ini file 
		 */
        void refresh_settings();

        [[nodiscard]] static Game& instance() { return *instance_; }

        /*!
		 * @brief Update logic function pointer
		 */
        std::function<void()> update_;

        SDLContext  sdlcontext_;
        Time        time_;
        Window      window_;
        AudioPlayer audio_player_;
        Renderer    renderer_ = Renderer(window_);
        GameScene   scene_;
        Settings    settings_;
        Profiler    profiler_;

        bool quit_ = false;

        float time_step_ = 1.0f / 60.0f;
        //bool step_by_step_	= false;
        //bool next_step_		= false;

        // There can be only 1 game instance tbh
        static inline Game* instance_ {nullptr};
    };
}    // namespace corgi
