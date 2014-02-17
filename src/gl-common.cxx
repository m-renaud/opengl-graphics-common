#include <mrr/graphics/gl-common.hxx>
#include <mrr/graphics/shader.hxx>
#include <mrr/graphics/obj_loader.hxx>

#include <iostream>
#include <string.h>

#define FOURCC_DXT1  0x31545844
#define FOURCC_DXT2  0x32545844
#define FOURCC_DXT3  0x33545844
#define FOURCC_DXT4  0x34545844
#define FOURCC_DXT5  0x35545844

namespace mrr {
namespace graphics {
namespace gl {

void init(float r, float g, float b, float a)
{
	::glClearColor(r, g, b, a);
	::glEnable(GL_DEPTH_TEST);
	::glDepthFunc(GL_LESS);
}

shader_handle colour_shader()
{
	return shader_handle(
		"/usr/local/share/mrr/graphics/shaders/colour-vertex-shader.glsl",
		"/usr/local/share/mrr/graphics/shaders/colour-fragment-shader.glsl"
	);
}


shader_handle texture_shader()
{
	return shader_handle(
		"/usr/local/share/mrr/graphics/shaders/texture-vertex-shader.glsl",
		"/usr/local/share/mrr/graphics/shaders/texture-fragment-shader.glsl"
	);
}


static GLuint loadDDS(const char * imagepath)
{
	unsigned char header[124];

	FILE *fp;

	/* try to open the file */
	fp = fopen(imagepath, "rb");
	if (fp == NULL)
		return 0;

	/* verify the type of file */
	char filecode[4];
	fread(filecode, 1, 4, fp);
	if (strncmp(filecode, "DDS ", 4) != 0) {
		fclose(fp);
		return 0;
	}

	/* get the surface desc */
	fread(&header, 124, 1, fp);

	unsigned int height      = *(unsigned int*)&(header[8 ]);
	unsigned int width         = *(unsigned int*)&(header[12]);
	unsigned int linearSize     = *(unsigned int*)&(header[16]);
	unsigned int mipMapCount = *(unsigned int*)&(header[24]);
	unsigned int fourCC      = *(unsigned int*)&(header[80]);

	unsigned char * buffer;
	unsigned int bufsize;
	/* how big is it going to be including all mipmaps? */
	bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
	buffer = (unsigned char*)malloc(bufsize * sizeof(unsigned char));
	fread(buffer, 1, bufsize, fp);
	/* close the file pointer */
	fclose(fp);

	unsigned int components  = (fourCC == FOURCC_DXT1) ? 3 : 4;
	unsigned int format;
	switch(fourCC)
	{
	case FOURCC_DXT1:
		format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		break;
	case FOURCC_DXT3:
		format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		break;
	case FOURCC_DXT5:
		format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		break;
	default:
		free(buffer);
		return 0;
	}

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);

	unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
	unsigned int offset = 0;

	/* load the mipmaps */
	for (unsigned int level = 0; level < mipMapCount && (width || height); ++level)
	{
		unsigned int size = ((width+3)/4)*((height+3)/4)*blockSize;
		glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,
		                       0, size, buffer + offset);

		offset += size;
		width  /= 2;
		height /= 2;
	}
	free(buffer);

	return textureID;
}


//m=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
shader_handle::shader_handle(
	::std::string const& vertex_shader_file,
	::std::string const& fragment_shader_file
)
	: vertex_shader_file_(vertex_shader_file),
	  fragment_shader_file_(fragment_shader_file),
	  shader_program_id_(
		  ::load_shaders(vertex_shader_file.c_str(), fragment_shader_file.c_str())
	  )
{
	if (shader_program_id_ == 0)
		std::exit(1);
}

shader_handle::~shader_handle()
{
	::glDeleteProgram(shader_program_id_);
}

void shader_handle::use()
{
	::glUseProgram(shader_program_id_);
}

GLuint shader_handle::get_program_id()
{
	return shader_program_id_;
}

GLuint shader_handle::get_uniform_location(char const* var_name)
{
	return ::glGetUniformLocation(get_program_id(), var_name);
}

//m=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
vertex_array::vertex_array()
{
	::glGenVertexArrays(1, &vertex_array_id_);
	bind();
}

vertex_array::~vertex_array()
{
	::glDeleteVertexArrays(1, &vertex_array_id_);
}

void vertex_array::bind()
{
	::glBindVertexArray(vertex_array_id_);
}


//m=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
buffer::buffer()
	: buffer_(0)
{
}

buffer::~buffer()
{
	if (buffer_ != 0)
		::glDeleteBuffers(1, &buffer_);
}

void buffer::create()
{
	::glGenBuffers(1, &buffer_);
}

void buffer::bind(GLenum target = GL_ARRAY_BUFFER)
{
	::glBindBuffer(target, buffer_);
}


//m=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
texture::texture()
	: texture_(0)
{
}

texture::~texture()
{
	if (texture_ != 0)
		::glDeleteTextures(1, &texture_);
}

void texture::load(::std::string const& filename)
{
	texture_ = loadDDS(filename.c_str());
}

void texture::bind(GLenum target = GL_TEXTURE_2D)
{
	::glActiveTexture(GL_TEXTURE0);
	::glBindTexture(target, texture_);
}


//m=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
model::model()
	: mvp_matrix_id_(0),
	  view_matrix_id_(0),
	  ambient_light_colour_id_(0),
	  point_source_locations_id_(0),
	  point_source_colours_id_(0),
	  point_source_powers_id_(0)
{
}

void model::set_shader(::mrr::graphics::gl::shader_handle const& s)
{
	shader_ = s;
	mvp_matrix_id_ = shader_.get_uniform_location("MVP");
	view_matrix_id_ = shader_.get_uniform_location("V");
	model_matrix_id_ = shader_.get_uniform_location("M");
}

void model::set_shader(
	::std::string const& vertex_shader_file,
	::std::string const& fragment_shader_file
)
{
	set_shader(shader_handle(vertex_shader_file, fragment_shader_file));
}

void model::add_component(model& m)
{
	components_.insert(&m);
	m.set_ambient_light_colour(ambient_light_colour_);
	for (int i = 0; i < point_source_locations_.size(); ++i)
		m.add_point_source(
			point_source_locations_[i],
			point_source_colours_[i],
			point_source_powers_[i]
		);
}

void model::remove_component(model& m)
{
	components_.erase(&m);
}

void model::update_model(::glm::mat4 const& t)
{
	for (model* m : components_)
		m->update_model(t);
}

void model::apply_fp_transformation(::glm::mat4 const& t)
{
	for (model* m : components_)
		m->apply_fp_transformation(t);
}

void model::set_ambient_light_colour(::glm::vec3 const& colour)
{
	ambient_light_colour_id_ = shader_.get_uniform_location("AmbientLightColour");
	ambient_light_colour_ = colour;

	for (model* m : components_)
		m->set_ambient_light_colour(colour);
}

::glm::vec3 const& model::get_ambient_light_colour() const
{
	return ambient_light_colour_;
}

GLuint model::get_ambient_light_colour_id() const
{
	return ambient_light_colour_id_;
}

void model::add_point_source(::glm::vec3 const& location, ::glm::vec3 const& colour, float power)
{
	point_source_locations_id_ = shader_.get_uniform_location("LightPosition_worldspace");
	point_source_colours_id_ = shader_.get_uniform_location("LightColour");
	point_source_powers_id_ = shader_.get_uniform_location("LightPower");
	point_source_count_id_ = shader_.get_uniform_location("PointSourceCount");

	point_source_locations_.push_back(location);
	point_source_colours_.push_back(colour);
	point_source_powers_.push_back(power);

	for (model* m : components_)
		m->add_point_source(location, colour, power);
}

GLuint model::get_point_source_locations_id() const
{
	return point_source_locations_id_;
}

GLuint model::get_point_source_colours_id() const
{
	return point_source_colours_id_;
}

GLuint model::get_point_source_powers_id() const
{
	return point_source_powers_id_;
}

GLuint model::get_point_source_count_id() const
{
	return point_source_count_id_;
}

std::vector<glm::vec3> const& model::get_point_source_locations() const
{
	return point_source_locations_;
}

std::vector<glm::vec3> const& model::get_point_source_colours() const
{
	return point_source_colours_;
}

std::vector<float> model::get_point_source_powers() const
{
	return point_source_powers_;
}

void model::save()
{
	for (model* m : components_)
		m->save();
}

void model::reset()
{
	for (model* m : components_)
		m->reset();
}

void model::render(::glm::mat4 const& V, ::glm::mat4 const& P)
{
	for (model* m : components_)
		m->render(V, P);
}




//m=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
component::component()
	:	texture_sampler_id_(0),
	  shape_colour_id_(0),
	  vertex_data_(nullptr),
	  colour_data_(nullptr),
	  uv_data_(nullptr),
	  normal_data_(nullptr),
	  va_size_(-1),
	  model_(::glm::mat4(1.0f)),
	  model_save_(::glm::mat4(1.0f))
{
}

::glm::vec3 const& component::get_location() const
{
	return location_;
}

void component::update_location()
{
	location_ = glm::vec3(model_ * glm::vec4(0, 0, 0, 1));
}

void component::set_vertex_data(GLfloat const* vertex_data, int size)
{
	std::cerr << "Setting vertex data: size = " << size << std::endl;
	va_size_ = size;
	vertex_data_ = vertex_data;

	vertex_buffer_.create();
	vertex_buffer_.bind(GL_ARRAY_BUFFER);
	::glBufferData(GL_ARRAY_BUFFER, va_size_, vertex_data_, GL_STATIC_DRAW);
}

void component::set_colour(::glm::vec3 const& shape_colour)
{
	shape_colour_id_ = shader_.get_uniform_location("shape_colour");
	shape_colour_ = shape_colour;
}

void component::set_colour_data(GLfloat const* colour_data)
{
	texture_sampler_id_ = shader_.get_uniform_location("texture_sampler");
	colour_data_ = colour_data;

	colour_buffer_.create();
	colour_buffer_.bind(GL_ARRAY_BUFFER);
	::glBufferData(GL_ARRAY_BUFFER, va_size_, colour_data_, GL_STATIC_DRAW);
}

void component::set_uv_data(GLfloat const* uv_data, int size)
{
	uv_data_ = uv_data;

	uv_buffer_.create();
	uv_buffer_.bind(GL_ARRAY_BUFFER);
	::glBufferData(GL_ARRAY_BUFFER, size, uv_data_, GL_STATIC_DRAW);
}

void component::set_normal_data(GLfloat const* normal_data, int size)
{
	normal_data_ = normal_data;

	normal_buffer_.create();
	normal_buffer_.bind(GL_ARRAY_BUFFER);
	::glBufferData(GL_ARRAY_BUFFER, size, normal_data_, GL_STATIC_DRAW);
}

void component::load_texture(::std::string const& filename)
{
	texture_.load(filename);
}

void component::set_init_model(::glm::mat4 const& m)
{
	init_model_ = m;
}

void component::set_model(::glm::mat4 const& m)
{
	model_ = m;
	update_location();
}

void component::update_model(::glm::mat4 const& t)
{
	model_ = t * model_;
	update_location();
}

void component::apply_fp_transformation(::glm::mat4 const& t)
{
	model_ = model_ * t;
	update_location();
}

void component::load_wavefront(std::string const& model_file)
{
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;

	bool successful = impl::load_wavefront(model_file, vertices, uvs, normals);
	if (!successful)
	{
		std::cerr << "Failed to load Wavefront OBJ file.\n";
		std::exit(1);
	}

	set_vertex_data(&vertices[0].x, vertices.size() * sizeof(glm::vec3));
	set_uv_data(&uvs[0].x, uvs.size() * sizeof(glm::vec2));
	set_normal_data(&normals[0].x, normals.size() * sizeof(glm::vec3));
}

void component::render(::glm::mat4 const& V, ::glm::mat4 const& P)
{
	::glm::mat4 MVP = P * V * model_;
	shader_.use();

	int ps_count = get_point_source_locations().size();

	::glUniform3fv(get_point_source_locations_id(), ps_count, &get_point_source_locations()[0].x);
	::glUniform3fv(get_point_source_colours_id(),   ps_count, &get_point_source_colours()[0].x);
	::glUniform1fv(get_point_source_powers_id(),    ps_count, &get_point_source_powers()[0]);
	::glUniform1i(get_point_source_count_id(), ps_count);

	::glUniform3fv(get_ambient_light_colour_id(), 1, &get_ambient_light_colour()[0]);

	::glUniformMatrix4fv(mvp_matrix_id_, 1, GL_FALSE, &MVP[0][0]);
	::glUniformMatrix4fv(model_matrix_id_, 1, GL_FALSE, &model_[0][0]);
	::glUniformMatrix4fv(view_matrix_id_, 1, GL_FALSE, &V[0][0]);

	if (uv_data_ != nullptr)
	{
		texture_.bind();
		::glUniform1i(texture_sampler_id_, 0);

		::glEnableVertexAttribArray(1);
		uv_buffer_.bind();
		::glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	}

	// Bind vertex attribute buffer...
	::glEnableVertexAttribArray(0);
	vertex_buffer_.bind();
	::glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);


	// NOTE: Colour data and normal/texture data must be mutually exclusize.
	if (colour_data_ != nullptr)
	{
		::glEnableVertexAttribArray(1);
		colour_buffer_.bind();
		::glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	}

	if (shape_colour_id_ != 0)
	{
		::glUniform3fv(shape_colour_id_, 1, &shape_colour_[0]);
	}

	if (normal_data_ != nullptr)
	{
		::glEnableVertexAttribArray(2);
		normal_buffer_.bind();
		::glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	}

	::glDrawArrays(GL_TRIANGLES, 0, va_size_);

	::glDisableVertexAttribArray(0);
	::glDisableVertexAttribArray(1);
	::glDisableVertexAttribArray(2);
}

void component::save()
{
	model_save_ = model_;
}

void component::reset()
{
	model_ = model_save_;
}


} // namespace gl
} // namespace graphics
} // namespace mrr


#undef FOURCC_DXT1
#undef FOURCC_DXT2
#undef FOURCC_DXT3
#undef FOURCC_DXT4
#undef FOURCC_DXT5
