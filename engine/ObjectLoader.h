#pragma once

#include <vector>
#include <string>

#include "glm/glm.hpp"
#include <GL/glew.h>
#include "GLFW/glfw3.h"

#include "Mesh.h"

namespace ObjectLoader {
	bool LoadObject
	(
		const char* path,
		MeshData& outMesh
	);

	GLuint LoadBMP(const char* imgPath);

	Mesh* SpawnObject(const MeshData&);
}
