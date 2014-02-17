#include <mrr/graphics/glfw-common.hxx>

#include <iostream>

namespace mrr {
namespace graphics {
namespace glfw {


//m=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
window_handle::window_handle(
	int width, int height, const char *title,
	GLFWmonitor* monitor, GLFWwindow* share
)
	: window_(::glfwCreateWindow(width, height, title, monitor, share))
{
	if (!window_)
	{
		::std::cerr << "Failed to create window.\n";
		::glfwTerminate();
		::exit(3);
	}

	set_key_callback(::mrr::graphics::glfw::key_callback);
	set_mouse_callback(::mrr::graphics::glfw::mouse_callback);

	make_current();
}

window_handle::~window_handle()
{
	close();
}

void window_handle::close()
{
	::glfwDestroyWindow(window_);
}

void window_handle::make_current()
{
	::glfwMakeContextCurrent(window_);
}

void window_handle::set_key_callback(GLFWkeyfun callback)
{
	::glfwSetKeyCallback(window_, callback);
}

void window_handle::set_mouse_callback(GLFWmousebuttonfun callback)
{
	::glfwSetMouseButtonCallback(window_, callback);
}

std::pair<double, double> window_handle::get_cursor_position() const
{
	std::pair<double, double> cursor_position;
	::glfwGetCursorPos(window_, &cursor_position.first, &cursor_position.second);
	return cursor_position;
}

void window_handle::set_framebuffer_size_callback(GLFWframebuffersizefun callback)
{
	::glfwSetFramebufferSizeCallback(window_, callback);
}

void window_handle::swap_buffers()
{
	::glfwSwapBuffers(window_);
}

bool window_handle::should_close()
{
	return ::glfwWindowShouldClose(window_);
}

GLFWwindow* window_handle::get()
{
	return window_;
}

//m=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
void init()
{
	if(!::glfwInit())
	{
		::std::cerr << "Failed to initialize GLFW\n";
		::exit(1);
	}

	::glfwSetErrorCallback(::mrr::graphics::glfw::default_error_callback);

	::glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	::glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
	::glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	::glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL

}

void default_error_callback(int error, const char* description)
{
	::std::cerr << "GLFW ERROR: " << std::string(description) << std::endl;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	::glViewport(0, 0, width, height);
}


//m=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Keyboard callbacks.
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	for (auto const& callback : key_callback_list)
		callback(window, key, scancode, action, mods);

	auto key_func_iter = keycode_to_function_map.find({action, key});
	if (key_func_iter != keycode_to_function_map.end())
		key_func_iter->second();
}

void add_key_callback(key_callback_type callback)
{
	key_callback_list.push_back(callback);
}

void add_key_callback(int action, int key, std::function<void()> const& func)
{
	keycode_to_function_map[{action, key}] = func;
}


//m=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Mouse callbacks.
void mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
	for (auto const& callback : mouse_callback_list)
		callback(window, button, action, mods);

	auto mouse_func_iter = mouse_click_to_function_map.find({action, button});
	if (mouse_func_iter != mouse_click_to_function_map.end())
		mouse_func_iter->second();
}

void add_mouse_callback(mouse_callback_type callback)
{
	mouse_callback_list.push_back(callback);
}

void add_mouse_callback(int action, int button, std::function<void()> const& func)
{
	mouse_click_to_function_map[{action, button}] = func;
}


//m=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Default key handlers.
namespace key_callbacks {

void quit_on_escape(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		::glfwSetWindowShouldClose(window, GL_TRUE);
}

} // namespace key_callbacks
} // namespace glfw
} // namespace graphics
} // namespace mrr
