#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <ios>
#include <ctime> 
#include <chrono>

#include "Light.h"
#include "Ray.h"
#include "Sphere.h"
#include "ppm.h"
#include "invert.h"


#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"


using namespace std;
using namespace glm;

const float cutoff = 0.0001f;

// read values from txt file
float NEAR, LEFT, RIGHT, TOP, BOTTOM;
int RES_NROWS, RES_NCOLS;
Sphere SPHERES[15];
int sphereCount;
Light LIGHTS[10];
int lightCount;
vec3 BACK, AMBIENT;
string OUTPUT;

vec3** pixels;

//camera variables
float W, H;
vec3 eye;

Ray GetRay(int row, int col)
{
	Ray ray = *new Ray();
	ray.start = eye;
	ray.direc = normalize(*new vec3(H * ((2.0 * row / RES_NROWS) - 1.0),
						- W * ((2.0 * col / RES_NCOLS) - 1.0),
						  -NEAR));
	return ray;
}

float doesCollide(Sphere sphere, Ray ray, float minValue, float& tValue)
{
	vec4 rayStart = vec4(ray.start, 1);
	vec4 rayDirec = vec4(ray.direc, 0);

	vec4 S4 = rayStart * sphere.mInv;
	vec3 S = vec3(S4);
	vec4 Direc4 = rayDirec * sphere.mInv;
	vec3 Direc = vec3(Direc4);
	float a = dot(Direc, Direc);
	float b = dot(S, Direc);
	float c = dot(S, S) - 1;
	float disc = ((b * b) - (a * c));
	if (disc < 0)
	{
		return false; // return negative value for no collisions.
	}
	else
	{
		float p1 = (-b / (a));
		float t1 = p1 + (sqrt(disc) / (a));
		float t2 = p1 - (sqrt(disc) / (a));

		tValue = (t1 > t2) ? ((t2 >= minValue) ? t2 : t1) : ((t1 >= minValue) ? t1 : t2);
		return true; // return the smaller of 2 values that is greater than passed in minValue.
	}
}

void initPixels()
{
	pixels = new vec3*[RES_NCOLS];
	for (int i = 0; i < RES_NCOLS; i++)
	{
		pixels[i] = new vec3[RES_NROWS];
	}
}

void calcCameraValues()
{
	W = (RIGHT - LEFT) / 2;
	H = (TOP - BOTTOM) / 2;
	eye = *new vec3(0, 0, 0);
}

vec3 getCollisionPoint(Sphere sphere, Ray ray, float tValue)
{
	return ray.start + (ray.direc * tValue);
}

vec3 raytrace(Ray ray)
{
	vec3 rgb = vec3(0, 0, 0); // black
	if (ray.currDepth == 0)
	{
		rgb = BACK; // if misses on first ray then it is background color
	}

	if (ray.currDepth >= 3)
	{
		return rgb;
	}

	bool foundCollision = false;
	Sphere closestCollision;
	float tValue = 0;
	float minValue = (ray.currDepth == 0) ? float(NEAR) : cutoff; // use min value of near if from eye, otherwise min dist
	
	for (int i = 0; i < sphereCount; i++)
	{
		float next;

		if (doesCollide(SPHERES[i], ray, minValue, next) && (next >= minValue)) // keep track of closest collision that's on the ray
		{
			if (foundCollision)
			{
				if (next < tValue)
				{
					tValue = next;
					closestCollision = SPHERES[i];
					rgb = closestCollision.rgbcolor;
				}
			}
			else
			{
				tValue = next;
				closestCollision = SPHERES[i];
				rgb = closestCollision.rgbcolor;
				foundCollision = true;
			}
		}
	}

	if (foundCollision) // then compute shadow ray, reflected and reftracted
	{
		if (closestCollision.position.z == -10.f)
		{
			 float h = 6.0;
		}
		//compute shadow rays
		vec3 collisionPoint = getCollisionPoint(closestCollision, ray, tValue);
		vec3 localIllumCol = closestCollision.reflcoefs[0] * AMBIENT * rgb;

		//calc norm for collision
		vec3 Norm = normalize(closestCollision.getNormal(collisionPoint));
		if (dot(Norm, ray.direc) > 0) // invert norm if hitting inside of sphere
		{
			Norm = -Norm;
		}

		for (int i = 0; i < lightCount; i++)
		{
			bool blocked = false;
			Ray lightRay;
			lightRay.start = collisionPoint;
			if ((closestCollision.position.z == -1) && (LIGHTS[i].position.z == -1))
			{
				bool br = true;
			}
			lightRay.direc = LIGHTS[i].position - collisionPoint;

			
			for (int j = 0; j < sphereCount; j++)
			{
				float tValue;
				if (doesCollide(SPHERES[j], lightRay, cutoff, tValue) && (tValue > cutoff) && (tValue < 1)) // if sphere blocks light
					//note: must be less than 1 so it doesnt count objects past the light
				{
					blocked = true;
				}
			}

			if (!blocked) 
			{
				//apply local illumination
				vec3 L = normalize(lightRay.direc);
				float nl = dot(L, Norm);
				vec3 diffuse = closestCollision.reflcoefs[1] * LIGHTS[i].intensity * glm::max(nl, 0.0f) * rgb;
				vec3 R = normalize(2.0f * dot(Norm, L) * Norm - L);
				vec3 V = normalize(-collisionPoint);
				float rv = dot(R, V);
				vec3 spec = closestCollision.reflcoefs[2] * LIGHTS[i].intensity * pow(glm::max(rv, 0.0f), closestCollision.specExp);


				localIllumCol += diffuse; // add diffuse component
				localIllumCol += spec; // add spec component
			}
		}

		// compute reflected rays
		
		Ray reflRay;
		reflRay.start = collisionPoint;
		reflRay.direc = normalize(- 2 * dot(Norm, ray.direc) * Norm + ray.direc);
		reflRay.currDepth = ray.currDepth + 1;
		vec3 reflColor = raytrace(reflRay);
		localIllumCol += closestCollision.reflcoefs[3] * reflColor; // add refl color

		return min(localIllumCol, 1.0f); // clamp to 1
	}

	return rgb;
}

void setPixels()
{
	for (int i = 0; i < RES_NCOLS; i++)
	{
		for (int j = 0; j < RES_NROWS; j++)
		{
			Ray ray = GetRay(i, j);
			pixels[i][j] = raytrace(ray);
		}
	}
}

int main(int argc, char* argv[])
{
	fstream readFile;
	if (argc < 2)
	{
		cout << "invalid number of command line args" << "\n";
		return -1;
	}

	auto start = std::chrono::system_clock::now();

	for (int i = 1; i < argc; i++)
	{
		readFile.open(argv[i], ios::in); // open read file
		//cout << "start" << "\n";
		sphereCount = 0;
		lightCount = 0;

		if (readFile.is_open())
		{
			sphereCount = 0;
			lightCount = 0;

			//cout << "opened" << "\n";

			string nextLine;
			while (!readFile.eof())
			{
				getline(readFile, nextLine);
				//cout << nextLine << "\n";
				stringstream lineStream;
				lineStream.str(nextLine);
				string nextTok;
				lineStream >> nextTok;

				if (nextTok == "NEAR")
				{
					lineStream >> nextTok;
					NEAR = stof(nextTok);
				}
				else if (nextTok == "LEFT")
				{
					lineStream >> nextTok;
					LEFT = stof(nextTok);
				}
				else if (nextTok == "RIGHT")
				{
					lineStream >> nextTok;
					RIGHT = stof(nextTok);
				}
				else if (nextTok == "BOTTOM")
				{
					lineStream >> nextTok;
					BOTTOM = stof(nextTok);
				}
				else if (nextTok == "TOP")
				{
					lineStream >> nextTok;
					TOP = stof(nextTok);
				}
				else if (nextTok == "RES")
				{
					lineStream >> nextTok;
					RES_NROWS = stoi(nextTok);

					lineStream >> nextTok;
					RES_NCOLS = stoi(nextTok);
				}
				else if (nextTok == "SPHERE")
				{
					Sphere newSphere;

					lineStream >> nextTok; // skip name

					//get position
					lineStream >> nextTok;
					float x = stof(nextTok);
					lineStream >> nextTok;
					float y = stof(nextTok);
					lineStream >> nextTok;
					float z = stof(nextTok);

					newSphere.position = *new vec3(x, y, z);

					//get scale
					lineStream >> nextTok;
					x = stof(nextTok);
					lineStream >> nextTok;
					y = stof(nextTok);
					lineStream >> nextTok;
					z = stof(nextTok);

					newSphere.scale = *new vec3(x, y, z);

					//get rgb
					lineStream >> nextTok;
					x = stof(nextTok);
					lineStream >> nextTok;
					y = stof(nextTok);
					lineStream >> nextTok;
					z = stof(nextTok);

					newSphere.rgbcolor = *new vec3(x, y, z);

					//get rgb
					lineStream >> nextTok;
					float ka = stof(nextTok);
					lineStream >> nextTok;
					float kd = stof(nextTok);
					lineStream >> nextTok;
					float ks = stof(nextTok);
					lineStream >> nextTok;
					float kr = stof(nextTok);

					newSphere.reflcoefs = *new vec4(ka, kd, ks, kr);

					//get exp
					lineStream >> nextTok;
					int n = stoi(nextTok);

					newSphere.specExp = (float)n;

					newSphere.createMatrixes();

					//add sphere
					SPHERES[sphereCount] = newSphere;
					sphereCount++;

				}
				else if (nextTok == "LIGHT")
				{
					Light newLight;

					lineStream >> nextTok; // skip name

					//get position
					lineStream >> nextTok;
					float x = stof(nextTok);
					lineStream >> nextTok;
					float y = stof(nextTok);
					lineStream >> nextTok;
					float z = stof(nextTok);

					newLight.position = *new vec3(x, y, z);

					//get intensity
					lineStream >> nextTok;
					x = stof(nextTok);
					lineStream >> nextTok;
					y = stof(nextTok);
					lineStream >> nextTok;
					z = stof(nextTok);

					newLight.intensity = *new vec3(x, y, z);

					//add light
					LIGHTS[lightCount] = newLight;
					lightCount++;
				}
				else if (nextTok == "BACK")
				{
					//get rgb
					lineStream >> nextTok;
					float r = stof(nextTok);
					lineStream >> nextTok;
					float g = stof(nextTok);
					lineStream >> nextTok;
					float b = stof(nextTok);

					BACK = *new vec3(r, g, b);
				}
				else if (nextTok == "AMBIENT")
				{
					//get rgb
					lineStream >> nextTok;
					float r = stof(nextTok);
					lineStream >> nextTok;
					float g = stof(nextTok);
					lineStream >> nextTok;
					float b = stof(nextTok);

					AMBIENT = *new vec3(r, g, b);
				}
				else if (nextTok == "OUTPUT")
				{
					//get output file name
					lineStream >> nextTok;
					OUTPUT = nextTok;
				}
			}

			readFile.close();

			auto indivStart = std::chrono::system_clock::now();
			initPixels();
			calcCameraValues();
			setPixels();
			auto indivEnd = std::chrono::system_clock::now();
			std::chrono::duration<double> elapsed_seconds = indivEnd - indivStart;
			cout << "elapsed time to complete image = " << elapsed_seconds.count() << "s\n";
			save_imageP3(RES_NCOLS, RES_NROWS, (char*)OUTPUT.c_str(), pixels);
		}
		else
		{
			cout << "error: bad read file";
		}
	}
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> total_elapsed_seconds = end - start;
	cout << "elapsed time to complete all tests = " << total_elapsed_seconds.count() << "s\n";
}