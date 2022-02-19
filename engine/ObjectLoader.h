#pragma once

#include <vector>
#include <string>

#include "glm/glm.hpp"
#include <GL/glew.h>
#include "GLFW/glfw3.h"

struct Mesh {
	std::string meshName;

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
};

namespace ObjectLoader {
	bool LoadObject
	(
		const char* path,
		Mesh& outMesh
	);

	GLuint LoadBMP(const char* imgPath);
}
