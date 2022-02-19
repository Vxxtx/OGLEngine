#pragma once

#include "glm/glm.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <algorithm>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <cstdlib>

class Renderer
{

public:
	/* Initialization with width and height */
	Renderer(int wWidth, int wHeight);
	~Renderer();

	int Begin();

protected:
	GLFWwindow* thisWindow{ nullptr };

	/* Window size */
	int width, height{ 800 };

	/* Assign sizes in double form as well for cursor calculation optimization */
	double d_width, d_height{ 800.0 };

	glm::mat4 getMatricesFromInput();

	glm::mat4 projMatrix{};
	glm::mat4 getProjectionMatrix() const { return projMatrix; }

	// Camera matrix
	glm::mat4 viewMatrix{};
	glm::mat4 getViewMatrix() const { return viewMatrix; }

	glm::vec3 position{ glm::vec3(0, 0, 5) };

	float hAngle{ 3.14f }; /* Horizontal angle */
	float vAngle{ 0.f }; /* Vertical angle */

	float FOV{ 45.f };
	float initialFOV{ 45.f };

	float speed{ 2.f };
	float mSpeed{ 0.5f };

	double lastTime{ 0.0 };

private:
	GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path);

	void calcInputs(GLFWwindow* window);
};
