#pragma once
#include "Object.h"
class Plane : public Object
{
public:
	Vector3 normal;
	Plane(Vector3 position, Vector3 normal, Material mat) : Object(position, mat), normal(normal)
	{

	}

	// Inherited via Object
	virtual bool Intersect(const Vector3& rayOrigin, const Vector3& rayDir, float& closeHitDist) const override;
	virtual Vector3 Normal(const Vector3& surfacePoint) const override;
};

