#include <mrr/graphics/gl-common.hxx>
#include <mrr/graphics/shader.hxx>

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

void init()
{
	::glEnable(GL_DEPTH_TEST);
	::glDepthFunc(GL_LESS);
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

void buffer::bind(GLenum target)
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

void texture::bind(GLenum target)
{
	::glBindTexture(target, texture_);
}


//m=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
void model::add_component(model& m)
{
	components_.push_back(&m);
}

void model::update_model(::glm::mat4 const& t)
{
	for (model* m : components_)
		m->update_model(t);
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

void model::render(::glm::mat4 const& VP)
{
	for (model* m : components_)
		m->render(VP);
}




//m=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
component::component()
	: mvp_matrix_id_(0),
	  texture_sampler_id_(0),
	  shape_colour_id_(0),
	  vertex_data_(nullptr),
	  colour_data_(nullptr),
	  uv_data_(nullptr),
	  va_size_(-1),
	  model_(::glm::mat4(1.0f)),
	  model_save_(::glm::mat4(1.0f))
{
}

void component::set_shader(::mrr::graphics::gl::shader_handle const& s)
{
	shader_ = s;
	mvp_matrix_id_ = shader_.get_uniform_location("MVP");
}

void component::set_shader(
	::std::string const& vertex_shader_file,
	::std::string const& fragment_shader_file
)
{
	set_shader(shader_handle(vertex_shader_file, fragment_shader_file));
}

void component::set_vertex_data(GLfloat const* vertex_data, int size)
{
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

void component::load_texture(::std::string const& filename)
{
	texture_.load(filename);
}

void component::set_model(::glm::mat4 const& m)
{
	model_ = m;
}

void component::update_model(::glm::mat4 const& t)
{
	model_ = t * model_;
}

void component::render(::glm::mat4 const& VP)
{
	::glm::mat4 MVP = VP * model_;
	shader_.use();
	::glUniformMatrix4fv(mvp_matrix_id_, 1, GL_FALSE, &MVP[0][0]);

	::glEnableVertexAttribArray(0);
	vertex_buffer_.bind(GL_ARRAY_BUFFER);
	::glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	if (uv_data_ != nullptr)
	{
		::glActiveTexture(GL_TEXTURE0);
		texture_.bind(GL_TEXTURE_2D);
		::glUniform1i(texture_sampler_id_, 0);

		::glEnableVertexAttribArray(1);
		uv_buffer_.bind(GL_ARRAY_BUFFER);
		::glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	}

	if (colour_data_ != nullptr)
	{
		::glEnableVertexAttribArray(1);
		colour_buffer_.bind(GL_ARRAY_BUFFER);
		::glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	}

	if (shape_colour_id_ != 0)
	{
		::glUniform3fv(shape_colour_id_, 1, &shape_colour_[0]);
	}

	::glDrawArrays(GL_TRIANGLES, 0, va_size_);

	::glDisableVertexAttribArray(0);
	::glDisableVertexAttribArray(1);
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
