#include "Plane.h"

bool Plane::Intersect(const Vector3& rayOrigin, const Vector3& rayDir, float& closeHitDist) const
{
	auto modN = normal * -1;
	// assuming vectors are all normalized
	float rayDotNormal = Vector3::dotProduct(modN, rayDir);
	if (rayDotNormal > 1e-6) {
		Vector3 p0l0 = position - rayOrigin;
		closeHitDist = Vector3::dotProduct(p0l0, modN) / rayDotNormal;
		return (closeHitDist >= 0);
	}
	return false;
}

Vector3 Plane::Normal(const Vector3& surfacePoint) const
{
	return normal;
}
