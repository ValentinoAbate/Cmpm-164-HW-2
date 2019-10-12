#include "Vector3.h"


// Returns the input vector normalized
// Code snippet adapted from: scrathapixels ray-tracing tutorial source:
// https://www.scratchapixel.com/code.php?id=8&origin=/lessons/3d-basic-rendering/ray-tracing-overview

Vector3 Vector3::normalize(const Vector3& v)
{
	float mag2 = v.x * v.x + v.y * v.y + v.z * v.z;
	if (mag2 > 0) {
		float invMag = 1 / sqrtf(mag2);
		return Vector3(v.x * invMag, v.y * invMag, v.z * invMag);
	}

	return v;
}

Vector3 Vector3::direction(const Vector3& from, const Vector3& to)
{
	return Vector3::normalize(from - to);
}

float Vector3::dotProduct(const Vector3& v1, const Vector3& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}
