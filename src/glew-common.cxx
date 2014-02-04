#include <mrr/graphics/glew-common.hxx>

#include <iostream>

namespace mrr {
namespace graphics {
namespace glew {

void init()
{
	::glewExperimental = GL_TRUE;
	if (::glewInit() != GLEW_OK) {
		::std::cerr << "Failed to initialize GLEW\n";
		::exit(2);
	}
}

} // namespace glew
} // namespace graphics
} // namespace mrr
