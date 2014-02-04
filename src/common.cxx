#include <mrr/graphics/common.hxx>

#include <iostream>
#include <functional>
#include <memory>
#include <vector>

#include <stdlib.h>


//m=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
namespace gl {

class shader_handle
{
public:
	shader_handle(::std::string vertex_shader_file, ::std::string fragment_shader_file)
		: vertex_shader_file_(vertex_shader_file),
		  fragment_shader_file_(fragment_shader_file),
		  shader_program_id_(
			  ::load_shaders(vertex_shader_file.c_str(), fragment_shader_file.c_str())
		  )
	{
	}

	void use()
	{
		::glUseProgram(shader_program_id_);
	}

private:
	GLuint shader_program_id_;
	::std::string vertex_shader_file_;
	::std::string fragment_shader_file_;
};


class vertex_array
{
public:
	vertex_array()
	{
		::glGenVertexArrays(1, &vertex_array_id_);
	}

	void bind()
	{
		::glBindVertexArray(vertex_array_id_);
	}

	~vertex_array()
	{
		::glDeleteVertexArrays(1, &vertex_array_id_);
	}

private:
	GLuint vertex_array_id_;
};


class vertex_buffer
{
public:
	vertex_buffer()
	{
		::glGenBuffers(1, &buffer_);
	}

	void bind(GLenum target)
	{
		::glBindBuffer(target, buffer_);
	}

	~vertex_buffer()
	{
		::glDeleteBuffers(1, &buffer_);
	}

private:
	GLuint buffer_;
};

} // namespace gl


//m=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Forward declarations.
namespace glfw {

using key_callback_type = std::function<void (GLFWwindow*, int, int, int, int)>;

void        init();
GLFWwindow* create_window(int, int, const char *, GLFWmonitor *, GLFWwindow *);
void        default_error_callback(int error, const char* description);
void        framebuffer_size_callback(GLFWwindow* window, int width, int height);
static void key_callback(GLFWwindow*, int, int, int, int);
static void add_key_callback(key_callback_type);

class window_handle;

namespace key_callbacks {

static void quit_on_escape(GLFWwindow*, int, int, int, int);

} // namespace key_callbacks

} // namespace glfw






//m=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
namespace glfw {

class window_handle
{
public:
	window_handle(
		int width, int height, const char *title,
		GLFWmonitor* monitor = NULL, GLFWwindow* share = NULL
	)
		: window_(::glfwCreateWindow(width, height, title, monitor, share))
	{
		if (!window_)
		{
			::std::cerr << "Failed to create window.\n";
			::glfwTerminate();
			::exit(3);
		}

		set_key_callback(::glfw::key_callback);
	}

	~window_handle()
	{
		close();
	}

	void close()
	{
		::glfwDestroyWindow(window_);
	}


	void make_current()
	{
		::glfwMakeContextCurrent(window_);
	}

	void set_key_callback(GLFWkeyfun callback)
	{
		::glfwSetKeyCallback(window_, callback);
	}

	void set_framebuffer_size_callback(GLFWframebuffersizefun callback)
	{
		::glfwSetFramebufferSizeCallback(window_, callback);
	}

	void swap_buffers()
	{
		::glfwSwapBuffers(window_);
	}

	bool should_close()
	{
		return ::glfwWindowShouldClose(window_);
	}

	GLFWwindow* get()
	{
		return window_;
	}

private:
	GLFWwindow* window_;
};


void init()
{
	if(!::glfwInit())
	{
		::std::cerr << "Failed to initialize GLFW\n";
		::exit(1);
	}

	::glfwSetErrorCallback(::glfw::default_error_callback);

	::glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	::glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
	::glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	::glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL

} // init()



GLFWwindow* create_window(
	int width, int height,
	const char *title,
	GLFWmonitor *monitor = NULL, GLFWwindow *share = NULL
)
{
	GLFWwindow* window = ::glfwCreateWindow(1920, 1080, "Simple Triangle", NULL, NULL);

	if (!window)
	{
		::std::cerr << "Failed to create window.\n";
		::glfwTerminate();
		::exit(3);
	}

	::glfwSetKeyCallback(window, ::glfw::key_callback);

	return window;
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
static std::vector<key_callback_type> key_callback_list;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	for (auto const& callback : key_callback_list)
		callback(window, key, scancode, action, mods);
}

static void add_key_callback(key_callback_type callback)
{
	key_callback_list.push_back(callback);
}


//m=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Default key handlers.
namespace key_callbacks {

static void quit_on_escape(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		::glfwSetWindowShouldClose(window, GL_TRUE);
}

} // namespace key_callbacks

} // namespace glfw

} // namespace mrr
