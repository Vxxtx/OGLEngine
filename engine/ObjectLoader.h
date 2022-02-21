#pragma once

#include <vector>
#include <string>

#include <fstream>
#include <iostream>
#include <sstream>

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

	GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path);

	Mesh* SpawnObject(const MeshData&);
}
