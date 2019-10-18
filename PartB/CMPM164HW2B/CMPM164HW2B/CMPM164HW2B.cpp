// CMPM164HW2B.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <limits>

#include "Object.h"
#include "Sphere.h"
#include "Plane.h"
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

float getShadowMod(const Vector3& rayOrigin, const Vector3& rayDirection, float lightDist, const std::unique_ptr<Object>& obj, 
	const std::vector<std::unique_ptr<Object>>& objects)
{
	// How much this light is unblocked
	float shadowMod = 1; 
	// Look through all objects to find the closest one that intersects
	for (size_t i = 0; i < objects.size(); ++i)
	{
		if (objects[i] == obj)
			continue;
		float hitDist;
		if (objects[i]->Intersect(rayOrigin, rayDirection, hitDist) && hitDist < lightDist)
		{
			shadowMod = fmaxf(shadowMod - objects[i]->mat.alpha, 0);
			if (shadowMod <= 0)
				return 0;
		}
	}
	return shadowMod;
}

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
			float lightDistance = Vector3::distance(intersectPoint, lights[i]->position);
			float shadowMod = getShadowMod(intersectPoint, lightVec, lightDistance, obj, objects);
			if (shadowMod == 0)
				continue;
			diffuseComponent += (obj->mat.color * lights[i]->color) * nDotL * shadowMod;
			auto viewVec = Vector3::direction(intersectPoint, cameraPos);
			auto reflectVec = Vector3::normalize((normal * (Vector3::dotProduct(lightVec, normal) * 2)) - lightVec);
			auto rDotV = Vector3::dotProduct(reflectVec, viewVec);
			if(rDotV > 0)
				specComponent += lights[i]->color * powf(rDotV, obj->mat.specExponent) * shadowMod;
		}
	}
	// return final light component clamped to be a valid color
	return Vector3::clamp(diffuseComponent + specComponent, 0, 1);
}

// Casts an initial ray that will eventually return a color to color a pixel with
Vector3 castRay(const Vector3& rayOrigin, const Vector3& rayDir, 
	const std::vector<std::unique_ptr<Object>>& objects, const std::vector<std::unique_ptr<Light>>& lights, 
	int depth, int maxDepth, Camera cam, int ignoreIndex = -1)
{
	if (depth == maxDepth)
		return cam.backgroundColor;
	float closeHitDist = 0;
	float minHitDist = std::numeric_limits<float>().infinity();
	int closestObjIndex = -1;

	// Look through all objects to find the closest one that intersects
	for (size_t i = 0; i < objects.size(); ++i)
	{
		if (i == ignoreIndex)
			continue;
		if (objects[i]->Intersect(rayOrigin, rayDir, closeHitDist) && closeHitDist < minHitDist)
		{
			closestObjIndex = i;
			minHitDist = closeHitDist;	
		}
	}
	// Ray hit an object, continue
	if (closestObjIndex >= 0)
	{
		auto hitPoint = rayOrigin + (rayDir * minHitDist);
		auto hitMat = objects[closestObjIndex]->mat;
		auto phongComponent = phongLighting(cam.position, objects[closestObjIndex], hitPoint, objects, lights);
		Vector3 retColor = phongComponent;
		if (hitMat.alpha < 1)
		{
			auto transparentComponent = castRay(hitPoint, rayDir, objects, lights, depth + 1, maxDepth, cam, closestObjIndex);
			retColor = Vector3::lerp(transparentComponent, phongComponent, hitMat.alpha);
		}
		if (hitMat.reflectance > 0)
		{
			auto normal = objects[closestObjIndex]->Normal(hitPoint);
			auto reflectVec = Vector3::reflect(rayDir, normal);
			auto reflectComponent = castRay(hitPoint, reflectVec, objects, lights, depth + 1, maxDepth, cam, closestObjIndex);
			retColor = Vector3::lerp(retColor, reflectComponent, hitMat.reflectance);
		}
		return retColor;
	}
	// Ray missed all objects, return the background color
	return cam.backgroundColor;
}

// renders a frame of output to an std::vector of Vector3s considered as colors and returns it
std::vector<Vector3> renderFrame(const Camera& cam, const std::vector<std::unique_ptr<Object>>& objects, const std::vector<std::unique_ptr<Light>>& lights)
{
	const int maxDepth = 9;
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
			frameBuffer[i * cam.imageHeightPix + j] = castRay(cam.position, rayDirection, objects, lights, 0, maxDepth, cam);
		}
	}

	return frameBuffer;
}

int main()
{
	Camera cam = Camera();
	cam.backgroundColor = Vector3(1, 0, 0);
	cam.imageWidthPix = 4096;
	cam.imageHeightPix = 4096;
	cam.imagePlaneTopLeft = Vector3(-0.5, -0.5, 0);
	cam.imagePlaneBottomRight = Vector3(0.5, 0.5, 0);
	cam.position = Vector3(0, 0, -2);

#pragma region Define Objects
	// Initialize object array
	std::vector<std::unique_ptr<Object>> objects = std::vector<std::unique_ptr<Object>>();

	// Create some materials
	Material matDefault = Material();
	matDefault.color = Vector3(1, 1, 1);
	matDefault.specExponent = 96;

	// Reflective mats
	Material matReflect = Material();
	matReflect.color = Vector3(0.75, 0.75, 0.75);
	matReflect.specExponent = 96;
	matReflect.reflectance = 0.75;

	// Transparent mats
	Material matHalfTransparent = Material();
	matHalfTransparent.color = Vector3(1, 1, 1);
	matHalfTransparent.alpha = 0.5;
	matHalfTransparent.specExponent = 96;
	matHalfTransparent.reflectance = 0.1;

	// Plane mats
	Material planeMatTopBottom = Material();
	planeMatTopBottom.color = Vector3(0, 0, 1);
	Material planeMatLR = Material();
	planeMatLR.color = Vector3(0, 1, 0);
	planeMatLR.reflectance = 0.45;
	Material planeMatBack = Material();
	planeMatBack.color = Vector3(1, 0, 0);
	planeMatBack.reflectance = 0.75;

	// Create and emplace objects
	Sphere* sphere1 = new Sphere(Vector3(0.25, -0.25, 1), 0.25, matReflect);
	objects.emplace_back(std::unique_ptr<Sphere>(sphere1));
	Sphere* sphere2 = new Sphere(Vector3(0.5, 0.5, 4), 0.25, matHalfTransparent);
	objects.emplace_back(std::unique_ptr<Sphere>(sphere2));
	Sphere* sphere3 = new Sphere(Vector3(-0.65, -0.23, 3), 0.25, matDefault);
	objects.emplace_back(std::unique_ptr<Sphere>(sphere3));

	// Define the top and bottom planes
	Plane* planeBottom = new Plane(Vector3(0, 0.75, 0), Vector3::normalize(Vector3(0, -1, 0)), planeMatTopBottom);
	objects.emplace_back(std::unique_ptr<Plane>(planeBottom));
	Plane* planeTop = new Plane(Vector3(0, -0.75, 0), Vector3::normalize(Vector3(0, 1, 0)), planeMatTopBottom);
	objects.emplace_back(std::unique_ptr<Plane>(planeTop));
	// Define the left and right planes
	Plane* planeLeft = new Plane(Vector3(-1, 0, 0), Vector3::normalize(Vector3(1, 0, 0)), planeMatLR);
	objects.emplace_back(std::unique_ptr<Plane>(planeLeft));
	Plane* planeRight = new Plane(Vector3(1, 0, 0), Vector3::normalize(Vector3(-1, 0, 0)), planeMatLR);
	objects.emplace_back(std::unique_ptr<Plane>(planeRight));
	// Define the front and back planes
	Plane* planeFront = new Plane(Vector3(0, 0, -5), Vector3::normalize(Vector3(0, 0, 1)), planeMatBack);
	objects.emplace_back(std::unique_ptr<Plane>(planeFront));
	Plane* planeBack = new Plane(Vector3(0, 0, 5), Vector3::normalize(Vector3(0, 0, -1)), planeMatBack);
	objects.emplace_back(std::unique_ptr<Plane>(planeBack));
#pragma endregion

#pragma region Define Lights
	std::vector<std::unique_ptr<Light>> lights = std::vector<std::unique_ptr<Light>>();
	Light* l1 = new Light(Vector3(0, -0.5, 3.5), Vector3(1, 1, 1), 1);
	lights.emplace_back(std::unique_ptr<Light>(l1));
	//Light* l2 = new Light(Vector3(2, 2, -1), Vector3(0.5, 1, 0.5), 1);
	//lights.emplace_back(std::unique_ptr<Light>(l2));
	Light* l3 = new Light(Vector3(0, 0, 0.5), Vector3(1, 1, 1), 1);
	lights.emplace_back(std::unique_ptr<Light>(l3));
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
