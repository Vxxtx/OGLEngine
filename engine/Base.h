#pragma once

#include <string>

#include "glm/glm.hpp"

class Base
{
public:
	Base();

	std::string name;

	glm::mat4 position{glm::mat4(1.0)};

	virtual void initialized();

	virtual void setWorldLocation(glm::vec3 inWorldLocation);
	virtual glm::vec3 getWorldLocation();
};

