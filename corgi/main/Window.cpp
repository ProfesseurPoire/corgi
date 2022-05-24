#include "Window.h"

#include <SDL2/SDL.h>
#include <corgi/logger/log.h>
#include <corgi/main/Game.h>
#include <corgi/math/Vec2i.h>
#include <corgi/ui/menu/MenuBar.h>
#include <glad/glad.h>

#include <mutex>
#include <stack>

namespace corgi
{
    class Renderer;

    void Window::initialize(
        const char* title, int x, int y, int width, int height, int monitor, bool vsync)
    {
        title_   = title;
        x_       = x;
        y_       = y;
        width_   = width;
        height_  = height;
        vsync_   = vsync;
        monitor_ = monitor;
    }

    void Window::setBorderless(bool value) { isBorderless_ = value; }

    bool Window::isBorderless() const noexcept { return isBorderless_; }

    // Could be used as a hack to be able to smoothly
    // resize the window, but I'm not too sure about this since it kinda
    // breaks the game loop since this only gets called when something moves
    // and is executed on a separate thread

    static std::mutex windowResizeMutex;

    struct WindowResize
    {
        int width, height;
    };

    static std::stack<WindowResize> windowResize;

    int filterEvent(void* userdata, SDL_Event* event)
    {
        if(event->type == SDL_WINDOWEVENT &&
           event->window.event == SDL_WINDOWEVENT_RESIZED)
        {
            auto window = static_cast<Window*>(userdata);

            // So this seems to kinda work?
            // So well, the idea is that ... since when we resize the window,
            // win32 doesn't send events.
            // Same function we got into Game, except we remove the input update
            if(event->window.data1 != window->width() ||
               event->window.data2 != window->height())
            {
                Game::instance().time_.update();
                while(Game::instance().time_.timestep_overrun())
                {
                    Game::instance().renderer().windowDrawList().clear();
                    Game::instance().renderer().world_draw_list().clear();

                    Game::instance().scene().before_update(
                        Game::instance().time_.timestep());

                    if(Game::instance().update_)
                        Game::instance().update_();

                    Game::instance().scene().update(Game::instance().time_.timestep());
                    Game::instance().scene().after_update(
                        Game::instance().time_.timestep());
                }
                Game::instance().window().resizeWithoutEvent(event->window.data1,
                                                             event->window.data2);
                Game::instance().window().resized(event->window.data1,
                                                  event->window.data2);

                Game::instance().renderer().draw_scene();
                Game::instance().window().swap_buffers();
            }

            //convert userdata pointer to yours and trigger your own draw function
            //this is called very often now
            //IMPORTANT: Might be called from a different thread, see SDL_SetEventFilter docs
            //return 0 if you don't want to handle this event twice
            return 0;
        }
        //important to allow all events, or your SDL_PollEvent doesn't get any event
        return 1;
    }

    Window::Window()
        : inputs_()
    {
        Cursor::init();
    }

    void Window::minimize()
    {
        SDL_MinimizeWindow(static_cast<SDL_Window*>(window_));
        isMinimized_ = true;
    }

    void Window::maximize() { SDL_MaximizeWindow(static_cast<SDL_Window*>(window_)); }
    void Window::restore() { SDL_RestoreWindow(static_cast<SDL_Window*>(window_)); }

    Window::~Window()
    {
        Cursor::finalize();
        SDL_GL_DeleteContext(opengl_context);
        SDL_DestroyWindow(static_cast<SDL_Window*>(window_));
    }

    void Window::addMenuBar(ui::MenuBar* menuBar) { menuBar_ = menuBar; }

    bool Window::isMinimized() const noexcept { return isMinimized_; }

    static SDL_HitTestResult SDLCALL hitTest(SDL_Window*      w,
                                             const SDL_Point* pt,
                                             void*            data)
    {
        corgi::Window* window = static_cast<corgi::Window*>(data);

        int borderSize = 5;

        if(window->menuBar() != nullptr)
        {
            if(pt->y > borderSize && pt->y < window->menuBar()->height() &&
               pt->x > window->menuBar()->menusWidth() &&
               pt->x < (window->menuBar()->width() - window->menuBar()->buttonsWidth()))
            {
                return SDL_HITTEST_DRAGGABLE;
            }
        }

        if(pt->x < borderSize)
        {
            if(pt->y < borderSize)
            {
                return SDL_HITTEST_RESIZE_TOPLEFT;
            }
            if(pt->y > (window->height() - borderSize))
            {
                return SDL_HITTEST_RESIZE_BOTTOMLEFT;
            }
            return SDL_HITTEST_RESIZE_LEFT;
        }

        if(pt->x > borderSize && pt->x < window->width() - borderSize)
        {
            if(pt->y < borderSize)
            {
                return SDL_HITTEST_RESIZE_TOP;
            }
            if(pt->y > (window->height() - borderSize))
            {
                return SDL_HITTEST_RESIZE_BOTTOM;
            }
        }

        if(pt->x > (window->width() - borderSize))
        {
            if(pt->y < borderSize)
            {
                return SDL_HITTEST_RESIZE_TOPRIGHT;
            }
            if(pt->y > (window->height() - borderSize))
            {
                return SDL_HITTEST_RESIZE_BOTTOMRIGHT;
            }
            return SDL_HITTEST_RESIZE_RIGHT;
        }

        return SDL_HITTEST_NORMAL;
    }

    ui::MenuBar* Window::menuBar() { return menuBar_; }

    void Window::show()
    {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

        uint32_t sdlFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;

        if(isBorderless())
        {
            sdlFlags = sdlFlags | SDL_WINDOW_BORDERLESS;
        }

        // Create an application window with the following settings:
        window_ = SDL_CreateWindow(title_.c_str(),    // window title
                                   x_,                // x position on the monitor
                                   y_,                // y position on the monitor
                                   width_,            // width, in pixels
                                   height_,           // height, in pixels
                                   sdlFlags
                                   // flags - see below
        );

        //SDL_CaptureMouse(SDL_bool::SDL_TRUE);

        //see filterEvent thing
        SDL_SetEventFilter(filterEvent, this);
        SDL_SetWindowHitTest(static_cast<SDL_Window*>(window_), hitTest, this);
        resizable(is_resizable());

        // Checks that the window was successfully created
        if(!window_)
        {
            log_error("Could not create window : " + std::string(SDL_GetError()));
            printf("Could not create window: %s\n", SDL_GetError());
        }

        // Creates the opengl context and make it current
        opengl_context = SDL_GL_CreateContext(static_cast<SDL_Window*>(window_));

        SDL_GL_MakeCurrent(static_cast<SDL_Window*>(window_), opengl_context);

        // glad initialization
        gladLoadGLLoader(static_cast<GLADloadproc>(SDL_GL_GetProcAddress));
        SDL_GL_SetSwapInterval(0);
    }

    Vec2i Window::dimensions() const { return Vec2i(width_, height_); }

    int Window::height() const { return height_; }

    int Window::width() const { return width_; }

    float Window::aspect_ratio() const
    {
        return static_cast<float>(width_) / static_cast<float>(height_);
    }

    int Window::x() const { return x_; }

    int Window::y() const { return height_; }

    bool Window::is_vsync() const { return vsync_; }

    void Window::title(const char* title) { title_ = title; }

    void Window::vsync(bool val) { vsync_ = val; }

    void Window::resizable(bool val)
    {
        if(val)
            SDL_SetWindowResizable(static_cast<SDL_Window*>(window_), SDL_TRUE);
        else
            SDL_SetWindowResizable(static_cast<SDL_Window*>(window_), SDL_FALSE);
        is_resizable_ = val;
    }

    bool Window::is_resizable() const noexcept { return is_resizable_; }

    bool Window::is_closing() const { return is_closing_; }

    Vec2i Window::position() const { return Vec2i(x_, y_); }

    void Window::setPosition(int x, int y)
    {
        x_ = x;
        y_ = y;
        SDL_SetWindowPosition(static_cast<SDL_Window*>(window_), x, y);
        swap_buffers();
    }

    void Window::swap_buffers() const
    {
        SDL_GL_SwapWindow(static_cast<SDL_Window*>(window_));
    }

    void Window::update()
    {
        inputs_.update();
        inputs_.keyboard_.mKeyModifiers = SDL_GetModState();
        poll_events();
    }

    void Window::resize(int width, int height)
    {
        width_  = width;
        height_ = height;
        SDL_SetWindowSize(static_cast<SDL_Window*>(window_), width, height);
    }

    void Window::poll_events()
    {
        SDL_Event e;
        float     val                = 0.0f;
        int       joystick_to_remove = -1;

        {
            std::lock_guard<std::mutex> guard(windowResizeMutex);
            while(!windowResize.empty())
            {
                auto windowSize = windowResize.top();
                windowResize.pop();
                width_  = windowSize.width;
                height_ = windowSize.height;
                resized(windowSize.width, windowSize.height);
                SDL_GL_SwapWindow(static_cast<SDL_Window*>(window_));
            }
        }
        SDL_Keycode kc;
        while(SDL_PollEvent(&e))
        {
            switch(e.type)
            {
                case SDL_WINDOWEVENT_ENTER:

                    break;

                case SDL_WINDOWEVENT:
                    switch(e.window.event)
                    {
                        case SDL_WINDOWEVENT_RESTORED:
                            isMinimized_ = false;
                            break;
                        case SDL_WINDOWEVENT_RESIZED:
                            width_  = e.window.data1;
                            height_ = e.window.data2;
                            resized(e.window.data1, e.window.data2);
                            SDL_GL_SwapWindow(static_cast<SDL_Window*>(window_));
                            return;
                            break;
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
                    inputs_.mouse_.x_ = e.motion.x;
                    inputs_.mouse_.y_ = e.motion.y;
                    break;

                case SDL_KEYDOWN:
                    inputs_.keyboard_._key_pressed_count++;

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

                    inputs_.gamepads_.emplace_back(e.jdevice.which);
                    break;

                case SDL_JOYDEVICEREMOVED:

                    for(int i = 0; i < inputs_.gamepads_.size(); i++)
                    {
                        if(inputs_.gamepads_[i].id_ == e.jdevice.which)
                        {
                            joystick_to_remove = i;
                        }
                    }

                    inputs_.gamepads_.remove_at(joystick_to_remove);

                    break;

                case SDL_JOYAXISMOTION:

                    if(e.jaxis.value >= 0)
                        val = static_cast<float>(e.jaxis.value) / 32767.0f;
                    else
                        val = static_cast<float>(e.jaxis.value) / 32768.0f;

                    inputs_.gamepads_[e.jaxis.which].axes_[e.jaxis.axis] = val;

                    break;

                case SDL_JOYBUTTONDOWN:

                    inputs_.gamepads_[e.jbutton.which].buttons_[e.jbutton.button] = true;
                    inputs_.gamepads_[e.jbutton.which].buttons_down_[e.jbutton.button] =
                        true;

                    break;

                case SDL_JOYBUTTONUP:

                    inputs_.gamepads_[e.jbutton.which].buttons_up_[e.jbutton.button] =
                        true;
                    inputs_.gamepads_[e.jbutton.which].buttons_[e.jbutton.button] = false;

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
                            inputs_.gamepads_[e.jhat.which].hat_ =
                                Gamepad::Hat::RightDown;
                            break;
                    }

                    break;

                case SDL_QUIT:

                    is_closing_ = true;
                    break;
            }
        }
    }
}    // namespace corgi
