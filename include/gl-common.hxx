#ifndef MRR_GRAPHICS_GL_COMMON_HXX__
#define MRR_GRAPHICS_GL_COMMON_HXX__

#include <mrr/graphics/glew-common.hxx>

#include <string>
#include <set>
#include <vector>

#include <glm/glm.hpp>

namespace mrr {
namespace graphics {
namespace gl {

void init(float r, float g, float b, float a);

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

	void set_ambient_light_colour(::glm::vec3 const& colour);

private:
	GLuint shader_program_id_;
	::std::string vertex_shader_file_;
	::std::string fragment_shader_file_;
};


shader_handle colour_shader();
shader_handle texture_shader();


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
	void destroy();
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
	void destroy();
	void bind(GLenum);
	bool is_loaded() const;

private:
	GLuint texture_;
	bool is_loaded_;
};


class model
{
public:
	model();

	void set_shader(::mrr::graphics::gl::shader_handle const& s);
	void set_shader(
		::std::string const& vertex_shader_file,
		::std::string const& fragment_shader_file
	);


	template <typename Iter>
	void add_components(Iter beg, Iter end)
	{
		for (; beg != end; ++beg)
			add_component(*beg);
	}

	void add_component(model& m);
	void remove_component(model& m);
	virtual void update_model(::glm::mat4 const& t);
	virtual void apply_fp_transformation(::glm::mat4 const& t);
	virtual void save();
	virtual void reset();

	void set_ambient_light_colour(::glm::vec3 const& colour);
	::glm::vec3 const& get_ambient_light_colour() const;

	GLuint get_ambient_light_colour_id() const;

	void add_point_source(::glm::vec3 const& location, ::glm::vec3 const& colour, float power);

	GLuint get_point_source_locations_id() const;
	GLuint get_point_source_colours_id() const;
	GLuint get_point_source_powers_id() const;
	GLuint get_point_source_count_id() const;

	std::vector<glm::vec3> const& get_point_source_locations() const;
	std::vector<glm::vec3> const& get_point_source_colours() const;
	std::vector<float> get_point_source_powers() const;

	virtual void render(::glm::mat4 const& V, ::glm::mat4 const& P);

protected:
	::mrr::graphics::gl::shader_handle shader_;

	GLuint mvp_matrix_id_;
	GLuint model_matrix_id_;
	GLuint view_matrix_id_;

	::glm::vec3 center_;
	::std::set<model*> components_;

	GLuint ambient_light_colour_id_;
	::glm::vec3 ambient_light_colour_;

	GLuint point_source_locations_id_;
	GLuint point_source_colours_id_;
	GLuint point_source_powers_id_;
	GLuint point_source_count_id_;

	::std::vector<glm::vec3> point_source_locations_;
	::std::vector<glm::vec3> point_source_colours_;
	::std::vector<float> point_source_powers_;
};


class component : public model
{
private:
	void update_location();

public:
	component();

	::glm::vec3 const& get_location() const;

	void set_vertex_data(GLfloat const* vertex_data, int size);
	void set_colour_data(GLfloat const* colour_data);
	void set_colour(::glm::vec3 const& shape_colour);
	void set_uv_data(GLfloat const* uv_data, int size);
	void set_normal_data(GLfloat const* normal_data, int size);
	void load_texture(::std::string const& filename);
	void set_init_model(::glm::mat4 const& m);
	void set_model(::glm::mat4 const& m);
	void update_model(::glm::mat4 const& t);
	void apply_fp_transformation(::glm::mat4 const& t);
	void load_wavefront(std::string const& path);
	void save();
	void reset();

	virtual void render(::glm::mat4 const& V, ::glm::mat4 const& P);

protected:
	GLuint texture_sampler_id_;

	std::vector<glm::vec3> vertices_;
	std::vector<glm::vec2> uvs_;
	std::vector<glm::vec3> normals_;

	GLuint shape_colour_id_;
	::glm::vec3 shape_colour_;

	::mrr::graphics::gl::buffer vertex_buffer_;
	GLfloat const* vertex_data_;

	::mrr::graphics::gl::buffer colour_buffer_;
	GLfloat const* colour_data_;

	::mrr::graphics::gl::buffer uv_buffer_;
	GLfloat const* uv_data_;

	::mrr::graphics::gl::buffer normal_buffer_;
	GLfloat const* normal_data_;

	::mrr::graphics::gl::texture texture_;

	int va_size_;

	::glm::vec3 location_;
	::glm::mat4 init_model_;
	::glm::mat4 model_;
	::glm::mat4 model_save_;
};


} // namespace gl
} // namespace graphics
} // namespace mrr

#endif // #ifndef MRR_GRAPHICS_GL_COMMON_HXX__
