// CMPM164HW2B.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <limits>

#include "Object.h"
#include "Sphere.h"
#include "Light.h"

#define M_PI 3.14159265358979323846264338327950288
static const float degToRad = M_PI / 180;
static const Vector3 colorWhite = Vector3(1, 1, 1);

struct Camera
{
	Vector3 position;
	uint32_t imageWidthPix;
	uint32_t imageHeightPix;
	Vector3 imagePlaneTopLeft;
	Vector3 imagePlaneBottomRight;
	Vector3 backgroundColor;
};

Vector3 phongLighting(const Vector3& cameraPos, const std::unique_ptr<Object>& obj, 
	const Vector3& intersectPoint, const std::vector<std::unique_ptr<Object>>& objects, 
	const std::vector<std::unique_ptr<Light>>& lights)
{
	Vector3 normal = obj->Normal(intersectPoint);
	Vector3 diffuseComponent = Vector3();
	Vector3 specComponent = Vector3();
	for(size_t i = 0; i < lights.size(); ++i)
	{
		auto lightVec = Vector3::direction(intersectPoint, lights[i]->position);
		auto nDotL = Vector3::dotProduct(lightVec, normal);
		if (nDotL > 0)
		{
			diffuseComponent += (obj->mat.color * lights[i]->color) * nDotL;
			auto viewVec = Vector3::direction(intersectPoint, cameraPos);
			auto reflectVec = (normal * (Vector3::dotProduct(lightVec, normal) * 2)) - lightVec;
			auto rDotV = Vector3::dotProduct(reflectVec, viewVec);
			if(rDotV > 0)
				specComponent += lights[i]->color * powf(rDotV, obj->mat.specExponent);
		}
	}

	// return final light component clamped to be a valid color
	return Vector3::clamp(diffuseComponent + specComponent, 0, 1);
}

// Casts an initial ray that will eventually return a color to color a pixel with
Vector3 castRay(const Vector3& rayOrigin, const Vector3& rayDir, 
	const std::vector<std::unique_ptr<Object>>& objects, const std::vector<std::unique_ptr<Light>>& lights, 
	int depth, int maxDepth, Vector3 backgroundColor)
{
	float closeHitDist = 0;
	float minHitDist = std::numeric_limits<float>().infinity();
	int closestObjIndex = -1;
	// Look through all objects to find the closest one that intersects
	for (size_t i = 0; i < objects.size(); ++i)
	{
		if (objects[i]->Intersect(rayOrigin, rayDir, closeHitDist) && closeHitDist < minHitDist)
		{
			closestObjIndex = i;
			minHitDist = closeHitDist;			
		}
	}
	// Ray hit an object, continue
	if (closestObjIndex >= 0)
	{
		//return objects[closestObjIndex]->mat.color;
		auto hitPoint = rayOrigin + (rayDir * closeHitDist);
		return phongLighting(rayOrigin, objects[closestObjIndex], hitPoint, objects, lights);
	}
	// Ray missed all objects, return the background color
	return backgroundColor;
}

// renders a frame of output to an std::vector of Vector3s considered as colors and returns it
std::vector<Vector3> renderFrame(const Camera& cam, const std::vector<std::unique_ptr<Object>>& objects, const std::vector<std::unique_ptr<Light>>& lights)
{
	const int maxDepth = 3;
	// Calculate pixel dimensions
	float widthPerPixel = abs((cam.imagePlaneTopLeft.x - cam.imagePlaneBottomRight.x) / cam.imageWidthPix);
	float heightPerPixel = abs((cam.imagePlaneTopLeft.y - cam.imagePlaneBottomRight.y) / cam.imageWidthPix);
	// Initialize an array
	std::vector<Vector3> frameBuffer = std::vector<Vector3>(cam.imageWidthPix * cam.imageHeightPix);
	for (size_t i = 0; i < cam.imageHeightPix; ++i)
	{
		for (size_t j = 0; j < cam.imageWidthPix; ++j)
		{
			// generate image plane position
			Vector3 imagePlanePos = Vector3(cam.imagePlaneTopLeft);
			imagePlanePos.x += widthPerPixel * (j + 0.5);
			imagePlanePos.y += widthPerPixel * (i + 0.5);
			// generate primary ray direction
			Vector3 rayDirection = Vector3::direction(cam.position, imagePlanePos);
			// Store the color that casting a ray returns
			frameBuffer[i * cam.imageHeightPix + j] = castRay(cam.position, rayDirection, objects, lights, 0, maxDepth, cam.backgroundColor);
		}
	}

	return frameBuffer;
}

int main()
{
	Camera cam = Camera();
	cam.backgroundColor = Vector3(1, 0, 0);
	cam.imageWidthPix = 800;
	cam.imageHeightPix = 800;
	cam.imagePlaneTopLeft = Vector3(-0.5, -0.5, 0);
	cam.imagePlaneBottomRight = Vector3(0.5, 0.5, 0);
	cam.position = Vector3(0, 0, -2);

#pragma region Define Objects
	// Initialize object array
	std::vector<std::unique_ptr<Object>> objects = std::vector<std::unique_ptr<Object>>();
	// Create some materials
	Material defaultMat = Material();
	defaultMat.color = Vector3(1, 1, 0.5);
	defaultMat.specExponent = 16;
	Material defaultMat2 = Material();
	defaultMat2.color = Vector3(1, 1, 1);
	defaultMat2.specExponent = 96;
	// Create and emplace objects
	Sphere* sphere1 = new Sphere(Vector3(0, 0, 1), 0.25, defaultMat);
	objects.emplace_back(std::unique_ptr<Sphere>(sphere1));
	Sphere* sphere2 = new Sphere(Vector3(0.8, -0.8, 3), 0.25, defaultMat2);
	objects.emplace_back(std::unique_ptr<Sphere>(sphere2));
#pragma endregion

#pragma region Define Lights
	std::vector<std::unique_ptr<Light>> lights = std::vector<std::unique_ptr<Light>>();
	Light* l1 = new Light(Vector3(0, 0.5, 0.5), Vector3(1, 1, 1), 1);
	lights.emplace_back(std::unique_ptr<Light>(l1));
	Light* l2 = new Light(Vector3(2, 2, 0.-1), Vector3(0.5, 1, 0.5), 1);
	lights.emplace_back(std::unique_ptr<Light>(l2));
#pragma endregion

	//render frame
	auto framebuffer = renderFrame(cam, objects, lights);

	//write frame to ppm file
	std::ofstream ofs;
	ofs.open("./out.ppm");
	ofs << "P6\n" << cam.imageWidthPix << " " << cam.imageHeightPix << "\n255\n\n";
	for (uint32_t i = 0; i < cam.imageHeightPix * cam.imageWidthPix; ++i) {
		char r = (char)(framebuffer[i].x * 255);
		char g = (char)(framebuffer[i].y * 255);
		char b = (char)(framebuffer[i].z * 255);
		if (r == '\n')
			++r;
		if (g == '\n')
			++g;
		if (b == '\n')
			++b;
		ofs << r << g << b;
	}
	ofs.close();
}
