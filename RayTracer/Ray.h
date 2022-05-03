#pragma once
#include "glm/vec3.hpp"


using namespace glm;

class Ray
{
public:
	vec3 start;
	vec3 direc;
	int currDepth;
};

