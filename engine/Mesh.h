#pragma once

#include "Base.h"

#include <vector>
#include <string>

#include <iostream>
#include <sstream>
#include <fstream>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"

#include <GL/glew.h>
#include "GLFW/glfw3.h"

struct MeshData {
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
};

class Mesh : public Base
{

public:
	~Mesh();

	void setMeshData(const MeshData& inMeshData);

	virtual void draw(const glm::mat4& projMatrix, const glm::mat4& viewMatrix);
protected:
	/* Vertex array object */
	GLuint vao{ 0 };
	GLuint shaderProgId{ 0 };

	GLuint matrixId{ 0 };
	GLuint viewMatrixId{ 0 };
	GLuint modelMatrixId{ 0 };

	GLuint objTexture{ 0 };
	GLuint textureId{ 0 };

	GLuint vertexBuf{ 0 };
	GLuint uvBuf{ 0 };
	GLuint normalBuf{ 0 };

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;

	virtual void initialized() override;

public:
	static bool LoadObject
	(
		const char* path,
		MeshData& outMesh
	);

	static Mesh SpawnObject(glm::vec3 world_position);

protected:
	GLuint LoadBMP(const char* imgPath);
	GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path);

};

