#include "Sphere.h"
#include <utility>

bool solveQuadratic(const float& a, const float& b, const float& c, float& x0, float& x1)
{
	float discr = b * b - 4 * a * c;
	if (discr < 0) return false;
	else if (discr == 0) x0 = x1 = -0.5 * b / a;
	else {
		float q = (b > 0) ?
			-0.5 * (b + sqrt(discr)) :
			-0.5 * (b - sqrt(discr));
		x0 = q / a;
		x1 = c / q;
	}
	if (x0 > x1) std::swap(x0, x1);
	return true;
}

bool Sphere::Intersect(const Vector3& rayOrigin, const Vector3& rayDir, float& closeHitDist) const
{
	// analytic solution
	Vector3 L = rayOrigin - position;
	float a = Vector3::dotProduct(rayDir, rayDir);
	float b = 2 * Vector3::dotProduct(rayDir, L);
	float c = Vector3::dotProduct(L, L) - radius * radius;
	float t0, t1;
	if (!solveQuadratic(a, b, c, t0, t1)) return false;
	if (t0 < 0) t0 = t1;
	if (t0 < 0) return false;
	closeHitDist = t0;
	return true;
}

Vector3 Sphere::Normal(const Vector3& surfacePoint) const
{
	return Vector3::direction(position, surfacePoint);
}
