#pragma once

#include <corgi/ecs/Scene.h>
#include <corgi/inputs/Inputs.h>
#include <corgi/main/AudioPlayer.h>
#include <corgi/main/Profiler.h>
#include <corgi/main/SDLContext.h>
#include <corgi/main/Settings.h>
#include <corgi/rendering/renderer.h>
#include <corgi/utils/Physic.h>
#include <corgi/utils/TimeHelper.h>

#include <functional>
#include <memory>
#include <string>
#include <vector>

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

    GameScene()                                  = default;
    GameScene(const GameScene& other)            = delete;
    GameScene(GameScene&& other)                 = delete;
    GameScene& operator=(const GameScene& other) = delete;
    GameScene& operator=(GameScene&& other)      = delete;
};

/*!
 * Contains every system needed for the game to launch. Must be initialized in the main of other project
 */
class Game
{
public:
    // Lifecycle

    Game();
    Game(const std::string& project_resource_directory,
         const std::string& library_resource_directory);
    ~Game();

    Game(const Game& other)            = delete;
    Game(Game&& other)                 = delete;
    Game& operator=(const Game& other) = delete;
    Game& operator=(Game&& other)      = delete;

    // Accessors

    [[nodiscard]] std::vector<std::unique_ptr<Window>>& windows();

    [[nodiscard]] GameScene& scene();

    [[nodiscard]] Renderer& renderer();
    [[nodiscard]] Settings& settings() noexcept;

    [[nodiscard]] const Inputs& inputs() const noexcept;

    [[nodiscard]] AudioPlayer& audio_player() noexcept;

    [[nodiscard]] const Profiler& profiler() const noexcept;
    [[nodiscard]] float           time_step();

    [[nodiscard]] Time&                                time() { return time_; }
    [[nodiscard]] std::function<void(Window& window)>& update() { return update_; }

    /*!
     * @brief   Returns a pointer to the window currently being drawn
     */
    [[nodiscard]] Window* current_window();

    // Functions

    void set_current_window(Window* window) { current_window_ = window; }

    /**
     * @brief Quit the game
     */
    void quit();

    /*!
	* @brief Starts the game loop
	*/
    void run();

    /*!
	* @brief Sets the fixed time between 2 logica update
	*/
    void time_step(float value);

    /*!
     * @brief   Creates a new window
     */
    Window& add_window(const std::string& name,
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

    //bool step_by_step_	= false;
    //bool next_step_		= false;

    // There can be only 1 game instance tbh
    static inline Game* instance_ {nullptr};

    /*!
     * @brief   Searches for a window that matches the given @p id parameter
     *
     * @details Loop through every window pointer opened and stored by the game
     *          and returns the first one that matches the given @p id.
     *          If no window could be found, returns nullptr
     * 
     * @param   id  Unique identifier of the window we're looking for
     *
     * @return  Returns a pointer to the window if we find a match, returns nullptr
     *          otherwise
     */
    [[nodiscard]] Window* find_window(unsigned int id);

private:
    void poll_events();

    // Variables

    /*!
	* @brief Update logic function pointer
	*/
    std::function<void(Window& window)> update_;

    SDLContext  sdlcontext_;
    Time        time_;
    AudioPlayer audio_player_;
    Renderer    renderer_;
    GameScene   scene_;
    Settings    settings_;
    Profiler    profiler_;
    Inputs      inputs_;

    float time_step_ = 1.0f / 60.0f;    // 4 bytes

    bool quit_ = false;    // 1 byte

    /*!
     * @brief Stores the windows currently opened
     */
    std::vector<std::unique_ptr<Window>> windows_;    // 32 bytes

    /*!
     * @brief   Pointer to the Window currently being drawn
     */
    Window* current_window_ {nullptr};    // 8 bytes
};
}    // namespace corgi
