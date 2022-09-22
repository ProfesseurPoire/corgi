#pragma once

#include <corgi/utils/Event.h>

#include <string>

namespace corgi
{
namespace ui
{
class MenuBar;
}

class Window
{
public:
    enum class GraphicAPI
    {
        Vulkan,
        OpenGL
    };

    enum class Flags
    {
        Borderless = 0b00000001,
        VSync      = 0b00000010
    };

    /*!
         *  @brief  Contains the data needed to init the window
         */
    struct CreateInfo
    {
        std::string title;
        int         x;
        int         y;
        int         width;
        int         height;
        GraphicAPI  graphic_api;
        bool        borderless;
        bool        vsync;
        bool        fullscreen;
        int         monitor {};
    };

    // Friends

    friend class Game;    // Here so game can call update() without making it public

    [[nodiscard]] static Window* current_window();

    // Lifecycle

    Window(CreateInfo descriptor);
    ~Window();

    Window(const Window& other)            = delete;
    Window(Window&& other)                 = delete;
    Window& operator=(const Window& other) = delete;
    Window& operator=(Window&& other)      = delete;

    /**
	 * @brief 	Adds a menuBar to the current window
	 * 
	 * @return ui::MenuBar* 
	*/
    void addMenuBar(corgi::ui::MenuBar* menuBar);

    /**
		 * @brief 	Gets the menu bar (if any)
		 * 
		 * @return const ui::MenuBar* 
		 */
    [[nodiscard]] corgi::ui::MenuBar* menuBar();

    void make_current();

    // Modifiers

    /*!
		 * @brief	Sets the window's title
		 * @param title	String to display on the window's title
		 */
    void title(const char* title);

    /*!
		 * @brief	Turns on or off VSync
		 *			Vsync means opengl will only draw after a monitor refresh
		 */
    void vsync(bool val);

    /*!
    	 * @brief	Resize the window
    	 */
    void resize(int width, int height);

    void resizable(bool val);

    // Accessors

    [[nodiscard]] bool is_resizable() const noexcept;

    // Functions

    void resizeWithoutEvent(int width, int height)
    {
        width_  = width;
        height_ = height;
    }

    /*!
    	 * @brief Actually shows the window
    	 */
    void show();

    struct Point
    {
        int x;
        int y;
    };
    /**
         * @brief   Minimize the current window into the task bar
         */
    void minimize();

    /**
         * @brief Maximize the current window to fit the whole screen
         */
    void maximize();

    /**
         * @brief Leave full screen/maximized mode to return back to windowed mode
         * 
         */
    void restore();

    /*!
		 * @brief	Returns the window's dimension (width and height)
		 */
    [[nodiscard]] Point dimensions() const;

    /*!
		 * @brief	Returns the window's position
		 */
    [[nodiscard]] Point position() const;

    /*!
		 * @brief	Returns the window's width
		 */
    [[nodiscard]] int width() const;

    /*!
		 * @brief	Returns the window's height
		 */
    [[nodiscard]] int height() const;

    /*!
		 * @brief	Returns the window's aspect ratio
		 */
    [[nodiscard]] float aspect_ratio() const;

    /**
         * @brief Tells the thing if the window should be borderless
         * 
         * @param value 
         */
    void setBorderless(bool value);

    /**
         * \brief 
         * \return 
         */
    [[nodiscard]] bool isBorderless() const noexcept;

    /*!
	 * @brief   Returns the window's top left x position relative to the screen
	 */
    [[nodiscard]] int x() const;

    /*!
	 * @brief   Returns the window's top left y position relative to the screen
	 */
    [[nodiscard]] int y() const;

    /*!
	 * @brief   Returns true if the window is currently using vsync 
	 *	    V-Sync means opengl will only draw after a monitor refresh (avoids artefacts where the monitor
	 *	    could be refreshed halfway while a frame was being constructed)
	 */
    [[nodiscard]] bool vsync() const noexcept;

    /*!
		 * @brief	Returns true when the window is currently in the process of closing, false otherwise
		 */
    [[nodiscard]] bool is_closing() const;

    void swap_buffers() const;

    /*!
         * @brief   Returns the window's id
         * 
         *          The id is given by SDL when creating the window
         * 
         * @return unsigned int 
         */
    [[nodiscard]] unsigned int id() const noexcept;
    /**
		 * @brief Set the window's position
		 * 
		 * @param x 
		 * @param y 
		 */
    void setPosition(int x, int y);

    /**
         * @brief When the window is minimized, we stop rendering entirely
         * 
         * @return true 
         * @return false 
         */
    [[nodiscard]] bool isMinimized() const noexcept;

    /**
         * \brief Gets the graphic api currently used by the window
         * \return 
         */
    [[nodiscard]] GraphicAPI graphic_api() const noexcept;

    void* sdl_window() { return window_; }

    // Events

    Event<int, int> resized;
    Event<int, int> moved;

private:
    // Functions

    // Member Variables

    std::string title_ {"window"};

    static inline Window* current_window_ {nullptr};

    int x_ {0};
    int y_ {0};
    int width_ {0};
    int height_ {0};
    int monitor_count_ {0};

    GraphicAPI graphic_api_;
    void*      window_ {nullptr};
    void*      opengl_context {nullptr};

    bool _running {false};

    // TODO : Implements the monitor thing. See #67
    int monitor_ {-1};    // if -1, it means the application is windowed

    // There is the option to share opengl context between windows but I'm not
    // sure how to actually do that so
    bool _shared {false};

    bool is_closing_ = false;
    bool is_resizable_ {true};
    bool isBorderless_ {false};
    bool fullscreen_ {false};
    bool vsync_ {false};

    friend class Game;

    corgi::ui::MenuBar* menuBar_ = nullptr;

    bool isMinimized_ = false;
};
}    // namespace corgi
