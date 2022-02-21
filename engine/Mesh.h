#pragma once

#include "Base.h"
#include <vector>
#include <string>

#include "glm/glm.hpp"
#include "GL/glew.h"
#include "GLFW/glfw3.h"

struct MeshData {
	std::string meshName;

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
};

class Mesh : public Base
{

public:
	~Mesh();

	void setMeshData(const MeshData& inMeshData);
	
	void draw(glm::mat4 projection_matrix, glm::mat4 view_matrix);
//protected:
	glm::mat4 w_pos;

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;

	GLuint vertex_array_object{ 0 };

	GLuint shader_program_id{ 0 };

	GLuint matrix_id{ 0 };
	GLuint view_matrix_id{ 0 };
	GLuint model_matrix_id{ 0 };

	GLuint obj_texture{ 0 };
	GLuint texture_id{ 0 };

	GLuint vertex_buf, uv_buf, normal_buf{ 0 };
	
	GLuint light_id{ 0 };

public:
	virtual void initialized() override;

};

