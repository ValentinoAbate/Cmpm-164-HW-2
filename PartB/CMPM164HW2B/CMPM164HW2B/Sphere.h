#pragma once
#include "Object.h"
class Sphere : public Object
{
public:
	float radius;
	Sphere(Vector3 position, float radius, Material mat) : Object(position, mat), radius(radius) 
	{
	}

	// Inherited via Object
	virtual bool Intersect(const Vector3& rayOrigin, const Vector3& rayDir, float& closeHitDist) const override;

	virtual Vector3 Normal(const Vector3& surfacePoint) const override;
};

