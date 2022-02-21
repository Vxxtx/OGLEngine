#pragma once

#include "Base.h"
#include <vector>
#include <string>
#include "glm/glm.hpp"

struct MeshData {
	std::string meshName;

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
};

class Mesh : public Base
{

public:
	Mesh();

	void setMeshData(const MeshData& inMeshData);
protected:
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;

	virtual void initialized() override;
};

