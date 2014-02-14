#ifndef MRR_GRAPHICS_GLFW_COMMON_HXX__
#define MRR_GRAPHICS_GLFW_COMMON_HXX__

#include <GLFW/glfw3.h>

#include <functional>
#include <map>
#include <vector>

namespace mrr {
namespace graphics {
namespace glfw {

using key_callback_type = ::std::function<void (GLFWwindow*, int, int, int, int)>;
using mouse_callback_type = ::std::function<void (GLFWwindow*, int, int, int)>;

static ::std::vector<key_callback_type> key_callback_list;
static ::std::map<std::pair<int,int>, std::function<void()> > keycode_to_function_map;

static ::std::vector<mouse_callback_type> mouse_callback_list;
static ::std::map<std::pair<int,int>, std::function<void()> > mouse_click_to_function_map;

void init();
void default_error_callback(int error, const char* description);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void key_callback(GLFWwindow*, int, int, int, int);
void add_key_callback(key_callback_type);
void add_key_callback(int, int, std::function<void()> const&);

void mouse_callback(GLFWwindow*, int, int, int);
void add_mouse_callback(mouse_callback_type);
void add_mouse_callback(int, int, std::function<void()> const&);


class window_handle
{
public:
	window_handle(
		int width, int height, const char *title,
		GLFWmonitor* monitor = NULL, GLFWwindow* share = NULL
	);

	~window_handle();
	void close();
	void make_current();
	void set_key_callback(GLFWkeyfun callback);
	void set_mouse_callback(GLFWmousebuttonfun callback);
	void set_framebuffer_size_callback(GLFWframebuffersizefun callback);
	void swap_buffers();
	bool should_close();

	template <typename LoopBody>
	void main_loop(LoopBody&& loop_body)
	{
		while (!should_close())
		{
			::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			loop_body();
			swap_buffers();
			::glfwPollEvents();
		}
	}

	GLFWwindow* get();

private:
	GLFWwindow* window_;
};


namespace key_callbacks {

void quit_on_escape(GLFWwindow* window, int key, int scancode, int action, int mods);

} // namespace key_callbacks
} // namespace glfw
} // namespace graphics
} // namespace mrr

#endif // #ifndef MRR_GRAPHICS_GLFW_COMMON_HXX__
