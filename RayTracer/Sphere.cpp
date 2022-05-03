#include "Sphere.h"
#include "invert.h"
#include "glm/gtc/matrix_inverse.hpp"

using namespace glm;


void Sphere::createMatrixes()
{
	mInv = inverse(mat4(scale.x, 0, 0, position.x, 
		0, scale.y, 0, position.y,
		0, 0, scale.z, position.z,
		0, 0, 0, 1));

	MInvT = mat4(1 / scale.x, 0, 0, 0,
				0, 1 / scale.y, 0, 0,
				0, 0, 1 / scale.z, 0,
				-position.x / scale.x, -position.y / scale.y, -position.z / scale.z, 1);
}

vec3 Sphere::getNormal(vec3 point)
{
	vec3 norm = 2.0f * (vec4((point - position), 1) * MInvT * MInvT);
	return norm;
}