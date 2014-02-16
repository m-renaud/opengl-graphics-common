#include <mrr/graphics/obj_loader.hxx>

#include <iostream>
#include <fstream>

namespace mrr {
namespace graphics {
namespace gl {
namespace impl {

bool load_wavefront(
	std::string const& path,
	std::vector<glm::vec3>& out_vertices,
	std::vector<glm::vec2>& out_uvs,
	std::vector<glm::vec3>& out_normals
)
{
	std::clog << "Loading OBJ file " << path << "...\n";

	std::vector<unsigned int> vertix_indices, uv_indices, normal_indices;
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;

	std::ifstream obj_file(path);

	std::string first_word;
	while(obj_file >> first_word)
	{
		if (first_word ==  "v")
		{
			glm::vec3 vertex;
			obj_file >> vertex.x >> vertex.y >> vertex.z;
			temp_vertices.push_back(vertex);
		}
		else if (first_word == "vt")
		{
			glm::vec2 uv;
			obj_file >> uv.x >> uv.y;

			// Invert V coordinate since we will only use DDS texture, which are inverted.
			// Remove if you want to use TGA or BMP loaders.
			uv.y = -uv.y;
			temp_uvs.push_back(uv);
		}
		else if (first_word == "vn")
		{
			glm::vec3 normal;
			obj_file >> normal.x >> normal.y >> normal.z;
			temp_normals.push_back(normal);
		}
		else if (first_word == "f")
		{
			char slash;
			std::string vertex1, vertex2, vertex3;
			unsigned int vertex_index[3], uv_index[3], normal_index[3];

			obj_file >> vertex_index[0] >> slash >> uv_index[0] >> slash >> normal_index[0]
			         >> vertex_index[1] >> slash >> uv_index[1] >> slash >> normal_index[1]
			         >> vertex_index[2] >> slash >> uv_index[2] >> slash >> normal_index[2];

			if (!obj_file)
			{
				std::cerr << "Parse error. Try exporting with other options\n";
				return false;
			}

			vertix_indices.push_back(vertex_index[0]);
			vertix_indices.push_back(vertex_index[1]);
			vertix_indices.push_back(vertex_index[2]);
			uv_indices    .push_back(uv_index[0]);
			uv_indices    .push_back(uv_index[1]);
			uv_indices    .push_back(uv_index[2]);
			normal_indices.push_back(normal_index[0]);
			normal_indices.push_back(normal_index[1]);
			normal_indices.push_back(normal_index[2]);
		}
		else
		{
			std::string garbage;
			std::getline(obj_file, garbage);
		}
	}

	// For each vertex of each triangle
	for(unsigned int i = 0; i < vertix_indices.size(); i++)
	{
		// Retrieve the attribute by index.
		glm::vec3 vertex = temp_vertices[vertix_indices[i]-1];
		glm::vec2 uv = temp_uvs[uv_indices[i]-1];
		glm::vec3 normal = temp_normals[normal_indices[i]-1];

		out_vertices.push_back(vertex);
		out_uvs     .push_back(uv);
		out_normals .push_back(normal);
	}

	return true;
}

} // namespace impl
} // namespace gl
} // namespace graphics
} // namespace mrr
