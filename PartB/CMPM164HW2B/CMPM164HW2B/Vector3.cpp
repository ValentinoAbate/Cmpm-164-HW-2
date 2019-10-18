#include "Vector3.h"
#include <cstdint>

float clampf(float value, float min, float max)
{
	if (value < min)
		return min;
	return value < max ? value : max;
}

float lerpf(float v1, float v2, float lerpFactor)
{
	return (1 - lerpFactor)* v1 + lerpFactor * v2;
}

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
	return Vector3::normalize(to - from);
}

const float Vector3::dotProduct(const Vector3& v1, const Vector3& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

Vector3 Vector3::clamp(const Vector3& vec, float min, float max)
{
	return Vector3(clampf(vec.x, min, max), clampf(vec.y, min, max), clampf(vec.z, min, max));
}

Vector3 Vector3::reflect(const Vector3& incident, const Vector3& normal)
{
	return incident - (normal * (dotProduct(incident, normal) * 2));
}

Vector3 Vector3::lerp(const Vector3& v1, const Vector3& v2, float lerpFactor)
{
	return Vector3(lerpf(v1.x, v2.x, lerpFactor), lerpf(v1.y, v2.y, lerpFactor), lerpf(v1.z, v2.z, lerpFactor));
}

Vector3 Vector3::crossProduct(const Vector3& v1, const Vector3& v2)
{
	float x = v1.y * v2.z - v1.z * v2.y;
	float y = v1.x * v2.z - v1.z * v2.x;
	float z = v1.x * v2.y - v1.y * v2.x;
	return Vector3(x, y, z);
}

float Vector3::distance(const Vector3& v1, const Vector3& v2)
{
	float dx = v1.x - v2.x;
	float dy = v1.y - v2.y;
	float dz = v1.z - v2.z;
	return sqrtf(dx * dx + dy * dy + dz * dz);
}
