#pragma once

#include <string>

class Base
{
public:
	Base();

	std::string name;

	virtual void initialized();
	virtual void setWorldLocation();
};

