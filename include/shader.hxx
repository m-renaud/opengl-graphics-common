#ifndef MRR_GRAPHICS_SHADER_HXX__
#define MRR_GRAPHICS_SHADER_HXX__

#include <GL/glew.h>

GLuint load_shaders(
	char const* vertex_file_path,
	char const* fragment_file_path
);

#endif // #ifndef MRR_GRAPHICS_SHADER_HXX__
