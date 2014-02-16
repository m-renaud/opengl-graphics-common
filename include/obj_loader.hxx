#ifndef WAVEFRONT_OBJ_LOADER_HXX__
#define WAVEFRONT_OBJ_LOADER_HXX__

#include <string>
#include <vector>

#include <glm/glm.hpp>

namespace mrr {
namespace graphics {
namespace gl {
namespace impl {

bool load_wavefront(
	std::string const& path,
	std::vector<glm::vec3>& out_vertices,
	std::vector<glm::vec2>& out_uvs,
	std::vector<glm::vec3>& out_normals
);

} // namespace impl
} // namespace gl
} // namespace graphics
} // namespace mrr


#endif // #ifndef WAVEFRONT_OBJ_LOADER_HXX__
