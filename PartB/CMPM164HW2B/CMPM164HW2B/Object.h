#pragma once

#include "Vector3.h"

struct Material
{
	Vector3 color = Vector3();
	float alpha = 1;
	float reflectance = 0;
	float refractance = 1;
	float diffuseComponent = 1;
	float specComponent = 1;
	float specExponent = 48;
};

class Object
{
public:
	Vector3 position;
	Material mat;
	Object(Vector3 position, Material mat) : position(position), mat(mat) {}
	// Returns true if given ray intersects with the object, else false
	// If the ray does intersect with the object, the distance to the closest intersection point 
	// is stored in float& closeHitDist 
	virtual bool Intersect(const Vector3& rayOrigin, const Vector3& rayDir, float& closeHitDist) const = 0;
	// Returns the normal at any point on the surface of an object
	virtual Vector3 Normal(const Vector3& surfacePoint) const = 0;
};

