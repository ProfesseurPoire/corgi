
#include <SDL2/SDL.h>
#include <corgi/inputs/Inputs.h>
#include <corgi/main/AudioPlayer.h>
#include <corgi/main/Game.h>
#include <corgi/main/Settings.h>
#include <corgi/main/Window.h>
#include <corgi/systems/SpriteRendererSystem.h>
#include <corgi/ui/UiUtils.h>
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

Game::Game(const std::string& project_resource_directory,
           const std::string& library_resource_directory)
{
    /* ResourcesCache::directories().emplace_back(project_resource_directory);
     ResourcesCache::directories().emplace_back(library_resource_directory);

     if(instance_)
         throw std::logic_error(
             "1 Game object was already initialized, there can be only one");
     instance_ = this;
     settings_.initialize("resources/Settings.ini");*/
}

Game::~Game()
{
    SpriteRendererSystem::release_sprite_mesh();
    UiUtils::release_nineslice_mesh();
}

Window* Game::find_window(unsigned int window_id)
{
    for(auto& window : windows_)
    {
        if(window->id() == window_id)
        {
            return window.get();
        }
    }
    return nullptr;
}

void Game::poll_events()
{
    SDL_Event e;
    float     val                = 0.0f;
    int       joystick_to_remove = -1;

    {
        // // TODO Why the fuck did I add a mutex here?

        // for(int i = 0; i < windows_.size(); i++)
        // {
        //     std::lock_guard<std::mutex> guard(windowResizeMutex);
        // }

        // while(!windowResize.empty())
        // {
        //     auto windowSize = windowResize.top();
        //     windowResize.pop();
        //     width_  = windowSize.width;
        //     height_ = windowSize.height;
        //     resized(windowSize.width, windowSize.height);
        //     SDL_GL_SwapWindow(static_cast<SDL_Window*>(window_));
        // }
    }
    SDL_Keycode kc;

    inputs_.mouse_.move_event_ = false;

    while(SDL_PollEvent(&e))
    {
        Window* window = find_window(e.window.windowID);

        switch(e.type)
        {
            case SDL_WINDOWEVENT_ENTER:

                break;

            case SDL_WINDOWEVENT:
                switch(e.window.event)
                {
                    case SDL_WINDOWEVENT_RESTORED:
                        window->isMinimized_ = false;
                        break;
                    case SDL_WINDOWEVENT_RESIZED:
                        window->width_  = e.window.data1;
                        window->height_ = e.window.data2;
                        window->resized(e.window.data1, e.window.data2);
                        SDL_GL_SwapWindow(static_cast<SDL_Window*>(window->window_));
                        return;
                }
                break;

            case SDL_MOUSEBUTTONUP:
                if(e.button.button == SDL_BUTTON_LEFT)
                {
                    inputs_.mouse_.buttons_up_[0] = true;
                    inputs_.mouse_.buttons_[0]    = false;
                }
                break;

            case SDL_MOUSEBUTTONDOWN:
                if(e.button.button == SDL_BUTTON_LEFT)
                {
                    inputs_.mouse_.buttons_[0]      = true;
                    inputs_.mouse_.buttons_down_[0] = true;
                }
                break;

            case SDL_MOUSEWHEEL:
                inputs_.mouse_.mWheelDelta = e.wheel.y;
                break;

            case SDL_MOUSEMOTION:

                inputs_.mouse_.previous_x_ = inputs_.mouse_.x_;
                inputs_.mouse_.previous_y_ = inputs_.mouse_.y_;
                inputs_.mouse_.x_          = e.motion.x;
                inputs_.mouse_.y_          = e.motion.y;
                inputs_.mouse_.move_event_ = true;
                break;

            case SDL_KEYDOWN:

                kc = e.key.keysym.sym;

                switch(e.key.keysym.sym)
                {
                    case SDLK_LEFT:
                        kc = static_cast<int>(corgi::Key::Left);
                        inputs_.keyboard_.keyPressed_.push_back(
                            static_cast<corgi::Key>(kc));
                        inputs_.keyboard_.mKeysDown[kc] = true;
                        inputs_.keyboard_.mKeys[kc]     = true;
                        break;
                    case SDLK_RIGHT:
                        kc = static_cast<int>(corgi::Key::Right);
                        inputs_.keyboard_.keyPressed_.push_back(
                            static_cast<corgi::Key>(kc));
                        inputs_.keyboard_.mKeysDown[kc] = true;
                        inputs_.keyboard_.mKeys[kc]     = true;
                        break;
                    case SDLK_UP:
                        kc = static_cast<int>(corgi::Key::Up);
                        inputs_.keyboard_.keyPressed_.push_back(
                            static_cast<corgi::Key>(kc));
                        inputs_.keyboard_.mKeysDown[kc] = true;
                        inputs_.keyboard_.mKeys[kc]     = true;
                        break;
                    case SDLK_DOWN:
                        kc = static_cast<int>(corgi::Key::Down);
                        inputs_.keyboard_.keyPressed_.push_back(
                            static_cast<corgi::Key>(kc));
                        inputs_.keyboard_.mKeysDown[kc] = true;
                        inputs_.keyboard_.mKeys[kc]     = true;
                        break;
                    case SDLK_LCTRL:
                        kc = 123;
                        break;
                    case SDLK_LSHIFT:
                        kc = 124;
                        break;
                    case SDLK_CAPSLOCK:
                        kc = 125;
                        break;
                }

                if(kc > 125)
                    break;

                inputs_.keyboard_.keyPressed_.push_back(static_cast<corgi::Key>(kc));
                inputs_.keyboard_.mKeysDown[kc] = true;
                inputs_.keyboard_.mKeys[kc]     = true;
                break;

            case SDL_KEYUP:

                kc = e.key.keysym.sym;

                switch(e.key.keysym.sym)
                {
                    case SDLK_LEFT:
                        kc = static_cast<int>(corgi::Key::Left);
                        inputs_.keyboard_._key_pressed_count--;
                        inputs_.keyboard_.mKeys[kc]   = false;
                        inputs_.keyboard_.mKeysUp[kc] = true;
                        break;
                    case SDLK_RIGHT:
                        kc = static_cast<int>(corgi::Key::Right);
                        inputs_.keyboard_._key_pressed_count--;
                        inputs_.keyboard_.mKeys[kc]   = false;
                        inputs_.keyboard_.mKeysUp[kc] = true;
                        break;
                    case SDLK_UP:
                        kc = static_cast<int>(corgi::Key::Up);
                        inputs_.keyboard_._key_pressed_count--;
                        inputs_.keyboard_.mKeys[kc]   = false;
                        inputs_.keyboard_.mKeysUp[kc] = true;
                        break;
                    case SDLK_DOWN:
                        kc = static_cast<int>(corgi::Key::Down);
                        inputs_.keyboard_._key_pressed_count--;
                        inputs_.keyboard_.mKeys[kc]   = false;
                        inputs_.keyboard_.mKeysUp[kc] = true;
                        break;
                    case SDLK_LCTRL:
                        kc = 123;
                        break;
                    case SDLK_LSHIFT:
                        kc = 124;
                        break;
                    case SDLK_CAPSLOCK:
                        kc = 125;
                        break;
                }

                if(kc > 125)
                    break;
                inputs_.keyboard_._key_pressed_count--;
                inputs_.keyboard_.mKeys[kc]   = false;
                inputs_.keyboard_.mKeysUp[kc] = true;
                break;

            case SDL_JOYDEVICEADDED:

                inputs_.gamepads_[e.jdevice.which].init(e.jdevice.which);
                break;

            case SDL_JOYDEVICEREMOVED:

                inputs_.gamepads_[e.jdevice.which].finalize();
                break;

            case SDL_JOYAXISMOTION:

                if(e.jaxis.value >= 0)
                    val = static_cast<float>(e.jaxis.value) / 32767.0f;
                else
                    val = static_cast<float>(e.jaxis.value) / 32768.0f;

                inputs_.gamepads_[e.jaxis.which].axes_[e.jaxis.axis] = val;

                break;

            case SDL_JOYBUTTONDOWN:
                inputs_.gamepads_[e.jbutton.which].buttons_[e.jbutton.button] =
                    Gamepad::Status::PressedAndDown;
                break;

            case SDL_JOYBUTTONUP:
                inputs_.gamepads_[e.jbutton.which].buttons_[e.jbutton.button] =
                    Gamepad::Status::Up;
                break;

            case SDL_JOYHATMOTION:

                switch(e.jhat.value)
                {
                    case SDL_HAT_LEFTUP:
                        inputs_.gamepads_[e.jhat.which].hat_ = Gamepad::Hat::LeftUp;
                        break;
                    case SDL_HAT_UP:
                        inputs_.gamepads_[e.jhat.which].hat_ = Gamepad::Hat::Up;
                        break;
                    case SDL_HAT_RIGHTUP:
                        inputs_.gamepads_[e.jhat.which].hat_ = Gamepad::Hat::RightUp;
                        break;
                    case SDL_HAT_LEFT:
                        inputs_.gamepads_[e.jhat.which].hat_ = Gamepad::Hat::Left;
                        break;
                    case SDL_HAT_CENTERED:
                        inputs_.gamepads_[e.jhat.which].hat_ = Gamepad::Hat::Centered;
                        break;
                    case SDL_HAT_RIGHT:
                        inputs_.gamepads_[e.jhat.which].hat_ = Gamepad::Hat::Right;
                        break;
                    case SDL_HAT_LEFTDOWN:
                        inputs_.gamepads_[e.jhat.which].hat_ = Gamepad::Hat::LeftDown;
                        break;
                    case SDL_HAT_DOWN:
                        inputs_.gamepads_[e.jhat.which].hat_ = Gamepad::Hat::Down;
                        break;
                    case SDL_HAT_RIGHTDOWN:
                        inputs_.gamepads_[e.jhat.which].hat_ = Gamepad::Hat::RightDown;
                        break;
                }

                break;

            case SDL_QUIT:

                quit_ = true;
                break;
        }
    }
}

float Game::time_step()
{
    return time_step_;
}

void Game::time_step(float value)
{
    time_step_ = value;
}

Renderer& Game::renderer()
{
    return renderer_;
}

Settings& Game::settings() noexcept
{
    return settings_;
}

const Inputs& Game::inputs() const noexcept
{
    return inputs_;
}

AudioPlayer& Game::audio_player() noexcept
{
    return audio_player_;
}

Window& Game::add_window(const std::string& name,
                         int                x,
                         int                y,
                         int                width,
                         int                height,
                         int                fullscreen,
                         bool               vsync,
                         bool               borderless)
{
    auto& window = windows_.emplace_back(std::make_unique<Window>());

    window->initialize(name.c_str(), x, y, width, height, fullscreen, vsync);
    window->setBorderless(borderless);
    window->show();

    return *window.get();
}

GameScene& Game::scene()
{
    return scene_;
}

std::vector<std::unique_ptr<Window>>& Game::windows()
{
    return windows_;
}

void Game::quit()
{
    quit_ = true;
}

Window* Game::current_window()
{
    return current_window_;
}

void Game::run()
{
    time_.timestep(time_step_);
    time_.start();

    profiler_.loop_counter_.start();

    std::cout << "Material Component Size : " << sizeof(Material) << std::endl;
    std::cout << "Vector Material Texture Uniform Size"
              << sizeof(std::vector<Material::TextureUniform>) << std::endl;

    while(!quit_)
    {
        inputs_.update();
        inputs_.keyboard_.mKeyModifiers = SDL_GetModState();
        poll_events();

        profiler_.update();
        time_.update();

        // I probably need a time for each window no?
        while(time_.timestep_overrun())
        {
            profiler_.update_counter_.start();

            renderer().windowDrawList().clear();
            renderer().world_draw_list().clear();
            scene_.before_update(time_.timestep());

            if(update_)
                update_(*windows_.front());

            scene_.update(time_.timestep());
            scene_.after_update(time_.timestep());

            profiler_.update_counter_.tick();
        }

        for(auto& window : windows_)
        {
            current_window_ = window.get();
            current_window_->make_current();
            // TODO : Remove the Game::current_window later on
            Window::current_window_ = current_window_;

            // The renderer needs to know the window to draw on
            renderer_.set_current_window(window.get());

            // Not sure if I should pack swap_buffer with it or not
            profiler_.renderer_counter_.start();
            renderer().draw_scene(*window.get(),
                                  dynamic_cast<Scene&>(Game::instance().scene()));
            profiler_.renderer_counter_.tick();

            window->swap_buffers();
            profiler_.loop_counter_.tick();
        }
    }

    // Clearing resources when exiting the main loop

    windows_.clear();
}

void Game::refresh_settings()
{
    settings_.refresh();
}

const Profiler& Game::profiler() const noexcept
{
    return profiler_;
}
}    // namespace corgi
