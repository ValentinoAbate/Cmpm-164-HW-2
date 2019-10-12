// CMPM164HW2B.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

#include "Object.h"
#include "Sphere.h"
#include "Light.h"

#define M_PI 3.14159265358979323846264338327950288
static const float degToRad = M_PI / 180;

struct Camera
{
	Vector3 position;
	uint32_t imageWidthPix;
	uint32_t imageHeightPix;
	Vector3 imagePlaneTopLeft;
	Vector3 imagePlaneBottomRight;
	Vector3 backgroundColor;
};

Vector3 castRay(const Vector3& rayOrigin, const Vector3& rayDir, const std::vector<std::unique_ptr<Object>>& objects, const std::vector<Light>& lights, int depth, int maxDepth, Vector3 backgroundColor)
{
	if (depth > maxDepth)
		return backgroundColor;
	float closeHitDist = 0;
	for (size_t i = 0; i < objects.size(); ++i)
	{
		if (objects[i]->Intersect(rayOrigin, rayDir, closeHitDist))
		{
			return objects[i]->mat.color;
		}
	}
	return backgroundColor;
}

// renders a frame of output to an std::vector of Vector3s considered as colors and returns it
std::vector<Vector3> renderFrame(const Camera& cam, const std::vector<std::unique_ptr<Object>>& objects, const std::vector<Light>& lights)
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
			// generate primary ray direction
			Vector3 imagePlanePos = Vector3(cam.imagePlaneTopLeft);
			imagePlanePos.x += widthPerPixel * (j + 0.5);
			imagePlanePos.y += widthPerPixel * (i + 0.5);
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
	cam.backgroundColor = Vector3(255, 1, 1);
	cam.imageWidthPix = 500;
	cam.imageHeightPix = 500;
	cam.imagePlaneTopLeft = Vector3(-1, -1, 0);
	cam.imagePlaneBottomRight = Vector3(1, 1, 0);
	cam.position = Vector3(0, 0, -1);

#pragma region Define Objects
	// Initialize object array
	std::vector<std::unique_ptr<Object>> objects = std::vector<std::unique_ptr<Object>>();
	// Create some materials
	Material defaultMat = Material();
	defaultMat.color = Vector3(1, 255, 1);
	Material defaultMat2 = Material();
	defaultMat2.color = Vector3(1, 1, 255);
	// Create and emplace objects
	Sphere* sphere1 = new Sphere(Vector3(0, 0, -2), 0.5, defaultMat);
	objects.emplace_back(std::unique_ptr<Sphere>(sphere1));
	Sphere* sphere2 = new Sphere(Vector3(2, 2, -3), 0.5, defaultMat2);
	objects.emplace_back(std::unique_ptr<Sphere>(sphere2));
#pragma endregion

#pragma region Define Lights
	std::vector<Light> lights = std::vector<Light>();
#pragma endregion

	//render frame
	auto framebuffer = renderFrame(cam, objects, lights);

	//write frame to ppm file
	std::ofstream ofs;
	ofs.open("./out.ppm");
	ofs << "P6\n" << cam.imageWidthPix << " " << cam.imageHeightPix << "\n255\n";
	for (uint32_t i = 0; i < cam.imageHeightPix * cam.imageWidthPix; ++i) {
		char r = (char)(framebuffer[i].x);
		char g = (char)(framebuffer[i].y);
		char b = (char)(framebuffer[i].z);
		ofs << g << b << r;
	}
	ofs.close();


}
