#include <corgi/main/Window.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <corgi/utils/log.h>

#include <corgi/rendering/renderer.h>

#include <corgi/inputs/Inputs.h>

namespace corgi
{
    static bool is_glfw_initialized_ = false;

	void glfw_error_callback(int, const char* error)
	{
		log_error("GLFW Error");
		log_error(error);
	}

	static bool initialize_glfw()
	{
		if (!is_glfw_initialized_)
		{
			log_message_on("Initializing GLFW Window", "Window");
			glfwSetErrorCallback(glfw_error_callback);

			if (!glfwInit())
			{
				log_error("GLFW could not be initialized");
				return false;
			}
			else
			{
				log_message_on("GLFW has been successfully initialized", "Window");
				is_glfw_initialized_ = true;
				return true;
			}
		}
		else
		{
			log_error("GLFW has already been initialized");
			// I do return false here because you probably should have never reach this part of the code
			return false;
		}
	}

	bool Window::create_window()
	{
		log_message_on("Creating " + title_ + " window","Window");

		GLFWmonitor** monitors = glfwGetMonitors(&monitor_count_);

		log_message_on("There's " + std::to_string(monitor_count_) + " monitor running", "Window");

		window_ = glfwCreateWindow(width_, height_, title_.c_str(), NULL, NULL);
		glfwSetWindowPos(static_cast<GLFWwindow*>(window_), x_, y_);

		if (!window_)
		{
			log_error("GLFW Window could not be created");
			return false;
		}
		log_message_on("GlFW window created", "Window");
		return true;
	}

   void Window::on_resized_event(GLFWwindow* window, int width, int height)
    {
	   Window* customWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	   customWindow->width_     = width;
	   customWindow->height_    = height;
        
       // Loop all weak_ptr stored
     //  for (int i = 0; i < customWindow->_resize_callbacks.size(); i++)
     //  {
     //      auto sp = customWindow->_resize_callbacks[i].lock();
     //      if (sp) // If a weak_ptr contains a delegate, we execute it
     //          (*sp)(width,height);
     //      else // otherwise we remove it from the list
			  //// customWindow->_resize_callbacks.erase(customWindow->_resize_callbacks.end() i--);
     //  }
    }

    void Window::on_moved_event(GLFWwindow* window, int x, int y)
    {
		Window* customWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

		customWindow->x_ = x;
		customWindow->y_ = y;

		//    // Loop all weak_ptr stored
		//    for (int i = 0; i < customWindow->_move_callbacks.size(); i++)
		//    {
		//        auto sp = customWindow->_move_callbacks[i].lock();
		//        if (sp) // If a weak_ptr contains a delegate, we execute it
		//            (*sp)(x, y);
		//        else // otherwise we remove it from the list
					//customWindow->_move_callbacks.remove_at(i--);
		//    }
    }

	float Window::aspect_ratio() const
	{
		return float(width_) / float(height_);
	}

	void Window::on_mouse_move_event(GLFWwindow*, double x, double y)
	{
		Inputs::non_const_mouse().previous_x_ = Inputs::non_const_mouse().x_;
		Inputs::non_const_mouse().previous_y_= Inputs::non_const_mouse().y_;
		Inputs::non_const_mouse().x_			= int(x);
		Inputs::non_const_mouse().y_			= int(y);

		Inputs::non_const_mouse().move_event_	= true;
	}

    void Window::on_mouse_button_event(GLFWwindow*, int button, int action,
	    int mods)
    {
	    switch (action)
	    {
		case GLFW_PRESS:
			Inputs::non_const_mouse().buttons_[button] = true;
			Inputs::non_const_mouse().buttons_down_[button] = true;

			break;

		case GLFW_RELEASE:
			Inputs::non_const_mouse().buttons_[button] = false;
			Inputs::non_const_mouse().buttons_up_[button] = true;
			break;
	    }
    }

	void Window::on_keyboard_event(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		switch (action)
		{
		case GLFW_PRESS:
			Inputs::non_const_keyboard()._keys_down[key] = true;
			Inputs::non_const_keyboard()._keys[key] = true;

			break;
		case GLFW_RELEASE:
			Inputs::non_const_keyboard()._keys[key] = false;
			Inputs::non_const_keyboard()._keys_up[key] = true;
			break;
		}
	}

    bool Window::initialize_callbacks()
	{
		// I'm binding my window object with the GLFWWindow
		glfwSetWindowUserPointer(static_cast<GLFWwindow*>(window_), this);

		// Binding 

		glfwSetWindowSizeCallback(static_cast<GLFWwindow*>(window_), on_resized_event);
		glfwSetWindowPosCallback(static_cast<GLFWwindow*>(window_), on_moved_event);
		glfwSetKeyCallback(static_cast<GLFWwindow*>(window_), on_keyboard_event);
		glfwSetCursorPosCallback(static_cast<GLFWwindow*>(window_), on_mouse_move_event);
		glfwSetMouseButtonCallback(static_cast<GLFWwindow*>(window_), on_mouse_button_event);
		return true;
	}

	Window::Window(const char* title, int x, int y, int width, int height, int monitor, bool vsync):
        width_(width),
        height_(height),
        x_(x),
        y_(y),
        title_(title),
		vsync_(vsync),
		monitor_(monitor)
	{
		initialize_glfw();
		create_window();

		// TODO : not sure this is still really actually needed or useful
		//			at the moment
		initialize_callbacks();

		// We first need to make the current context current
		// before loading glad 
		glfwMakeContextCurrent(static_cast<GLFWwindow*>(window_));

		// This initialize GLAD stuff so probably should call that
		// before the context thing?
		gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		_running = true;
	}

    Window::~Window()
    {
        glfwDestroyWindow(static_cast<GLFWwindow*>(window_));
        glfwTerminate();
    }

    bool Window::is_closing()const
    {
        return glfwWindowShouldClose(static_cast<GLFWwindow*>(window_));
    }

    void Window::show()
    {
		
    }

    Vec2i Window::dimensions() const
    {
		return Vec2i(width_, height_);
    }

    Vec2i Window::position() const
    {
		return Vec2i(x_, y_);
    }

    int Window::width()const
    {
        return width_;
    }

    int Window::height()const
    {
        return height_;
    }

    int Window::x()const
    {
        return x_;
    }

    int Window::y()const
    {
        return y_;
    }

    void Window::resize(int width, int height)
    {
        width_  = width;
        height_ = height;
        glfwSetWindowSize(static_cast<GLFWwindow*>(window_), width, height);
    }

	

	void Window::swap_buffers()const
    {
        glfwSwapBuffers(static_cast<GLFWwindow*>(window_));
    }

    void* Window::underlying_window()const
    {
        return window_;
    }

    void Window::add_resize_callback(std::shared_ptr<ResizeWindowCallback> delegate)
    {
        _resize_callbacks.push_back(delegate);
    }

    void Window::add_move_callback(std::shared_ptr<MoveCallback> delegate)
    {
        _move_callbacks.push_back(delegate);
    }


    void Window::vsync(bool value)
    {
        value? glfwSwapInterval(1) : glfwSwapInterval(0);
        vsync_ = value;
    }
    bool Window::is_vsync() const
    {
        return  vsync_;
    }

    void Window::title(const char* title)
    {
		if(_running)
			glfwSetWindowTitle(static_cast<GLFWwindow*>(window_), title);
    }
}