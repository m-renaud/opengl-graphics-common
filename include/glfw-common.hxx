#ifndef MRR_GRAPHICS_GLFW_COMMON_HXX__
#define MRR_GRAPHICS_GLFW_COMMON_HXX__

#include <mrr/graphics/waypoint.hxx>
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
	std::pair<double, double> get_cursor_position() const;
	void set_framebuffer_size_callback(GLFWframebuffersizefun callback);

	void add_waypoint(::mrr::graphics::gl::waypoint const& wp);

	template <typename Iter>
	void add_waypoints(Iter begin, Iter end)
	{
		for (; begin != end; ++begin)
			add_waypoint(*begin);
	}

	void process_waypoints() const;
	void swap_buffers();
	bool should_close();

	double get_ms_per_frame() const;

	template <typename LoopBody>
	void main_loop(LoopBody&& loop_body)
	{
		last_time = ::glfwGetTime();
		nb_frames = 0;

		while (!should_close())
		{
			current_time = ::glfwGetTime();
			++nb_frames;

			if (current_time - last_time >= 1)
			{
				ms_per_frame = 1000.0 / static_cast<double>(nb_frames);
				nb_frames = 0;
				last_time += (current_time - last_time);
			}

			::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			process_waypoints();
			loop_body();
			swap_buffers();
			::glfwPollEvents();
		}
	}

	GLFWwindow* get();

private:
	GLFWwindow* window_;
	::std::vector<::mrr::graphics::gl::waypoint> waypoints_;

	double last_time;
	double current_time;
	int nb_frames;
	double ms_per_frame;
};


namespace key_callbacks {

void quit_on_escape(GLFWwindow* window, int key, int scancode, int action, int mods);

} // namespace key_callbacks
} // namespace glfw
} // namespace graphics
} // namespace mrr

#endif // #ifndef MRR_GRAPHICS_GLFW_COMMON_HXX__
