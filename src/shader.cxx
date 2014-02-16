#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>

#include <stdlib.h>
#include <string.h>

#include <GL/glew.h>

#include <mrr/graphics/shader.hxx>

static std::string file_contents_to_string(std::ifstream& in)
{
	std::string file_contents;
	in.seekg(0, std::ios::end);
	file_contents.resize(in.tellg());
	in.seekg(0, std::ios::beg);
	in.read(&file_contents[0], file_contents.size());
	in.close();

	return file_contents;
}


GLuint load_shaders(
	const char* vertex_shader_path,
	const char* fragment_shader_path
)
{
	GLuint vertex_shader_id = ::glCreateShader(GL_VERTEX_SHADER);
	GLuint fragment_shader_id = ::glCreateShader(GL_FRAGMENT_SHADER);

	// Read vertex shader code from file.
	std::string vertex_shader_code;
	std::ifstream vertex_shader_stream(vertex_shader_path);

	if (vertex_shader_stream)
	{
		vertex_shader_code = file_contents_to_string(vertex_shader_stream);
	}
	else
	{
		std::cerr << "Cannot open vertex shader!!!\tpath: " << vertex_shader_path << std::endl;
		return 0;
	}

	// Read fragment shader code from file.
	std::string fragment_shader_code;
	std::ifstream fragment_shader_stream(fragment_shader_path);

	if (fragment_shader_stream)
	{
		fragment_shader_code = file_contents_to_string(fragment_shader_stream);
	}
	else
	{
		std::cerr << "Cannot open fragment shader!!!\tpath: " << fragment_shader_path << std::endl;
		return 0;
	}


	GLint result = GL_FALSE;
	int info_log_length;

	// Compile the vertex shader.
	std::clog << "Compiling vertex shader...\tpath: " << vertex_shader_path << std::endl;
	const char* vertex_source_ptr = vertex_shader_code.c_str();
	::glShaderSource(vertex_shader_id, 1, &vertex_source_ptr, NULL);
	::glCompileShader(vertex_shader_id);

	// Check that the vertex shader was loaded correctly.
	::glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &result);
	::glGetShaderiv(vertex_shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
	if (info_log_length > 0)
	{
		std::vector<char> vertex_shader_error_message(info_log_length+1);
		::glGetShaderInfoLog(vertex_shader_id, info_log_length, NULL, &vertex_shader_error_message[0]);
		std::cerr << std::string(&vertex_shader_error_message[0]) << std::endl;
	}


	// Compile the fragment shader.
	std::clog << "Compiling fragment shader...\tpath: " << fragment_shader_path << std::endl;
	const char* fragment_source_ptr = fragment_shader_code.c_str();
	::glShaderSource(fragment_shader_id, 1, &fragment_source_ptr, NULL);
	::glCompileShader(fragment_shader_id);

	// Check that the fragment shader was loaded correctly.
	::glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &result);
	::glGetShaderiv(fragment_shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
	if (info_log_length > 0)
	{
		std::vector<char> fragment_shader_error_message(info_log_length+1);
		::glGetShaderInfoLog(fragment_shader_id, info_log_length, NULL, &fragment_shader_error_message[0]);
		std::cerr << std::string(&fragment_shader_error_message[0]) << std::endl;
	}

	// Link the shader program.
	std::clog << "Linking program...\n";
	GLuint program_id = ::glCreateProgram();
	::glAttachShader(program_id, vertex_shader_id);
	::glAttachShader(program_id, fragment_shader_id);
	::glLinkProgram(program_id);

	// Check the program for correctness.
	::glGetProgramiv(program_id, GL_LINK_STATUS, &result);
	::glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_log_length);
	if (info_log_length > 0)
	{
		std::vector<char> program_error_message(info_log_length+1);
		::glGetProgramInfoLog(program_id, info_log_length, NULL, &program_error_message[0]);
		std::cerr << std::string(&program_error_message[0]) << std::endl;
	}

	::glDeleteShader(vertex_shader_id);
	::glDeleteShader(fragment_shader_id);

	return program_id;
}
