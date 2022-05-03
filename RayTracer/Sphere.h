#pragma once
#include "glm/glm.hpp"

using namespace glm;

class Sphere
{
public:
	vec3 position;
	vec3 scale;
	vec3 rgbcolor;
	vec4 reflcoefs;
	float specExp;
	mat4 mInv;
	mat4 MInvT;

	void createMatrixes();

	vec3 getNormal(vec3 point);

};

