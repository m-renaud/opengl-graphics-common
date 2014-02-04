#ifndef MRR_GRAPHICS_GL_COMMON_HXX__
#define MRR_GRAPHICS_GL_COMMON_HXX__

#include <mrr/graphics/glew-common.hxx>

#include <string>
#include <vector>

#include <glm/glm.hpp>

namespace mrr {
namespace graphics {
namespace gl {

void init();

class shader_handle
{
public:
	shader_handle() = default;
	shader_handle(shader_handle const&) = default;
	shader_handle(shader_handle&&) = default;

	shader_handle& operator =(shader_handle const&) = default;
	shader_handle& operator =(shader_handle&&) = default;

	~shader_handle();

	shader_handle(
		::std::string const& vertex_shader_file,
		::std::string const& fragment_shader_file
	);

	void use();
	GLuint get_program_id();
	GLuint get_uniform_location(char const* var_name);

private:
	GLuint shader_program_id_;
	::std::string vertex_shader_file_;
	::std::string fragment_shader_file_;
};



class vertex_array
{
public:
	vertex_array();
	vertex_array(vertex_array const&) = default;
	vertex_array(vertex_array&&) = default;

	vertex_array& operator =(vertex_array const&) = default;
	vertex_array& operator =(vertex_array&&) = default;

	~vertex_array();

	void bind();

private:
	GLuint vertex_array_id_;
};



class buffer
{
public:
	buffer();
	buffer(buffer const&) = default;
	buffer(buffer&&) = default;

	buffer& operator =(buffer const&) = default;
	buffer& operator =(buffer&&) = default;

	~buffer();

	void create();
	void bind(GLenum target);

private:
	GLuint buffer_;
};


class texture
{
public:
	texture();
	~texture();

	void load(::std::string const& filename);
	void bind(GLenum);

private:
	GLuint texture_;
};


class model
{
public:
	model() = default;

	void add_component(model& m);
	virtual void update_model(::glm::mat4 const& t);
	virtual void save();
	virtual void reset();

	virtual void render(::glm::mat4 const& VP);

private:
	::glm::vec3 center_;
	std::vector<model*> components_;
};


class component : public model
{
public:
	component();

	void set_shader(::mrr::graphics::gl::shader_handle const& s);
	void set_shader(
		::std::string const& vertex_shader_file,
		::std::string const& fragment_shader_file
	);

	void set_vertex_data(GLfloat const* vertex_data, int size);
	void set_colour_data(GLfloat const* colour_data);
	void set_colour(::glm::vec3 const& shape_colour);
	void set_uv_data(GLfloat const* uv_data, int size);
	void load_texture(::std::string const& filename);
	void set_model(::glm::mat4 const& m);
	void update_model(::glm::mat4 const& t);
	void save();
	void reset();

	virtual void render(::glm::mat4 const& VP);

private:
	::mrr::graphics::gl::shader_handle shader_;
	GLuint mvp_matrix_id_;
	GLuint texture_sampler_id_;

	GLuint shape_colour_id_;
	::glm::vec3 shape_colour_;

	::mrr::graphics::gl::buffer vertex_buffer_;
	GLfloat const* vertex_data_;

	::mrr::graphics::gl::buffer colour_buffer_;
	GLfloat const* colour_data_;

	::mrr::graphics::gl::buffer uv_buffer_;
	GLfloat const* uv_data_;

	::mrr::graphics::gl::texture texture_;

	int va_size_;

	::glm::mat4 model_;
	::glm::mat4 model_save_;
};


} // namespace gl
} // namespace graphics
} // namespace mrr

#endif // #ifndef MRR_GRAPHICS_GL_COMMON_HXX__
