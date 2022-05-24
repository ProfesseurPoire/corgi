#pragma once

#include <corgi/ShortString.h>
#include <corgi/SimpleString.h>
#include <corgi/inputs/Inputs.h>
#include <corgi/math/Vec2i.h>
#include <corgi/ui/menu/MenuBar.h>
#include <corgi/utils/Event.h>

namespace corgi
{
    namespace ui
    {
        class MenuBar;
    }

    class Window
    {
    public:
        // Friends

        friend class Game;    // Here so game can call update() without making it public

        // Lifecycle

        Window();

        ~Window();

        Window(const Window& other) = delete;
        Window(Window&& other)      = delete;
        Window& operator=(const Window& other) = delete;
        Window& operator=(Window&& other) = delete;

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

        void initialize(const char* title,
                        int         x,
                        int         y,
                        int         width,
                        int         height,
                        int         monitor,
                        bool        vsync);

        /*!
    	 * @brief Actually shows the window
    	 */
        void show();

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
        [[nodiscard]] Vec2i dimensions() const;

        /*!
		 * @brief	Returns the window's position
		 */
        [[nodiscard]] Vec2i position() const;

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

        [[nodiscard]] bool isBorderless() const noexcept;

        /*!
		 * @brief	Returns the window's top left x position relative to the screen
		 */
        [[nodiscard]] int x() const;

        /*!
		 * @brief	Returns the window's top left y position relative to the screen
		 */
        [[nodiscard]] int y() const;

        /*!
		 * @brief	Returns true if the window is currently using vsync 
		 *			V-Sync means opengl will only draw after a monitor refresh (avoids artefacts where the monitor
		 *			could be refreshed halfway while a frame was being constructed)
		 */
        [[nodiscard]] bool is_vsync() const;

        /*!
		 * @brief	Returns true when the window is currently in the process of closing, false otherwise
		 */
        [[nodiscard]] bool is_closing() const;

        void swap_buffers() const;

        void poll_events();

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

        // Events

        Event<int, int> resized;
        Event<int, int> moved;

    private:
        // Functions

        void update();

        // Member Variables

        Inputs inputs_;

        ShortString title_ {"window"};

        int x_ {0};
        int y_ {0};
        int width_ {0};
        int height_ {0};
        int monitor_count_ {0};

        void* window_ {nullptr};
        void* opengl_context {nullptr};

        bool _running {false};
        bool vsync_ {false};

        // TODO : Implements the monitor thing. See #67
        int monitor_ {-1};    // if -1, it means the application is windowed

        // There is the option to share opengl context between windows but I'm not
        // sure how to actually do that so
        bool _shared {false};

        bool is_closing_ = false;
        bool is_resizable_ {true};
        bool isBorderless_ = false;

        friend class Game;

        corgi::ui::MenuBar* menuBar_ = nullptr;

        bool isMinimized_ = false;
    };
}    // namespace corgi
