#include <SDL2/SDL.h>
#include <corgi/logger/log.h>
#include <corgi/main/Game.h>
#include <corgi/main/Window.h>
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

void Window::setBorderless(bool value)
{
    isBorderless_ = value;
}

bool Window::isBorderless() const noexcept
{
    return isBorderless_;
}

Window* Window::current_window()
{
    return current_window_;
}

unsigned int Window::id() const noexcept
{
    return SDL_GetWindowID(static_cast<SDL_Window*>(window_));
}

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
    if(event->type == SDL_WINDOWEVENT && event->window.event == SDL_WINDOWEVENT_RESIZED)
    {
        auto window = static_cast<Window*>(userdata);

        // So this seems to kinda work?
        // So well, the idea is that ... since when we resize the window,
        // win32 doesn't send events.
        // Same function we got into Game, except we remove the input update
        if(event->window.data1 != window->width() ||
           event->window.data2 != window->height())
        {
            Game::instance().time().update();
            while(Game::instance().time().timestep_overrun())
            {
                Game::instance().renderer().windowDrawList().clear();
                Game::instance().renderer().world_draw_list().clear();

                Game::instance().scene().before_update(
                    Game::instance().time().timestep());

                if(Game::instance().update())
                    Game::instance().update()(*window);

                Game::instance().scene().update(Game::instance().time().timestep());
                Game::instance().scene().after_update(Game::instance().time().timestep());
            }
            window->resizeWithoutEvent(event->window.data1, event->window.data2);
            window->resized(event->window.data1, event->window.data2);

            Game::instance().renderer().draw_scene(*window);
            window->swap_buffers();
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

Window::Window() {}

void Window::minimize()
{
    SDL_MinimizeWindow(static_cast<SDL_Window*>(window_));
    isMinimized_ = true;
}

void Window::maximize()
{
    SDL_MaximizeWindow(static_cast<SDL_Window*>(window_));
}

void Window::restore()
{
    SDL_RestoreWindow(static_cast<SDL_Window*>(window_));
}

Window::~Window()
{
    Cursor::finalize();
    SDL_GL_DeleteContext(opengl_context);
    SDL_DestroyWindow(static_cast<SDL_Window*>(window_));
}

void Window::addMenuBar(ui::MenuBar* menuBar)
{
    menuBar_ = menuBar;
}

bool Window::isMinimized() const noexcept
{
    return isMinimized_;
}

static SDL_HitTestResult SDLCALL hitTest(SDL_Window* w, const SDL_Point* pt, void* data)
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

ui::MenuBar* Window::menuBar()
{
    return menuBar_;
}

static void* opengl_context_ = nullptr;

void Window::show()
{
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    uint32_t sdlFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;

    if(isBorderless())
    {
        sdlFlags = sdlFlags | SDL_WINDOW_BORDERLESS;
    }

    //SDL_GL_SHARE_WITH_CURRENT_CONTEXT

    SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);

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

    // glad initialization

    static bool init = false;

    if(!init)
    {
        opengl_context_ = SDL_GL_CreateContext(static_cast<SDL_Window*>(window_));

        SDL_GL_MakeCurrent(static_cast<SDL_Window*>(window_), opengl_context_);

        init = true;
        gladLoadGLLoader(static_cast<GLADloadproc>(SDL_GL_GetProcAddress));
    }

    SDL_GL_SetSwapInterval(0);
}

void Window::make_current()
{
    SDL_GL_MakeCurrent(static_cast<SDL_Window*>(window_), opengl_context_);
}

Window::Point Window::dimensions() const
{
    return Point {width_, height_};
}

int Window::height() const
{
    return height_;
}

int Window::width() const
{
    return width_;
}

float Window::aspect_ratio() const
{
    return static_cast<float>(width_) / static_cast<float>(height_);
}

int Window::x() const
{
    return x_;
}

int Window::y() const
{
    return height_;
}

bool Window::is_vsync() const
{
    return vsync_;
}

void Window::title(const char* title)
{
    title_ = title;
}

void Window::vsync(bool val)
{
    vsync_ = val;
}

void Window::resizable(bool val)
{
    if(val)
        SDL_SetWindowResizable(static_cast<SDL_Window*>(window_), SDL_TRUE);
    else
        SDL_SetWindowResizable(static_cast<SDL_Window*>(window_), SDL_FALSE);
    is_resizable_ = val;
}

bool Window::is_resizable() const noexcept
{
    return is_resizable_;
}

bool Window::is_closing() const
{
    return is_closing_;
}

Window::Point Window::position() const
{
    return Point {x_, y_};
}

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

void Window::resize(int width, int height)
{
    width_  = width;
    height_ = height;
    SDL_SetWindowSize(static_cast<SDL_Window*>(window_), width, height);
}
}    // namespace corgi
