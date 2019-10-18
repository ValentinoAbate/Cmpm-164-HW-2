#pragma once
#include <cmath>
class Vector3
{	
public: 
	float x, y, z;
	Vector3() : x(0), y(0), z(0)
	{

	}
	Vector3(float xInit, float yInit, float zInit) : x(xInit), y(yInit), z(zInit)
	{

	}

#pragma region Operator Overloads
	// Code copied from: scrathapixels ray-tracing tutorial source:
	Vector3 operator * (const float& scalar) const { return Vector3(x * scalar, y * scalar, z * scalar); }
	Vector3 operator * (const Vector3& v) const { return Vector3(x * v.x, y * v.y, z * v.z); }
	Vector3 operator - (const Vector3& v) const { return Vector3(x - v.x, y - v.y, z - v.z); }
	Vector3 operator + (const Vector3& v) const { return Vector3(x + v.x, y + v.y, z + v.z); }
	Vector3 operator - () const { return Vector3(-x, -y, -z); }
	Vector3& operator += (const Vector3& v) { x += v.x, y += v.y, z += v.z; return *this; }
#pragma endregion

	// Returns the input vector normalized
	// Code snippet adapted from: scrathapixels ray-tracing tutorial source:
	// https://www.scratchapixel.com/code.php?id=8&origin=/lessons/3d-basic-rendering/ray-tracing-overview
	static Vector3 normalize(const Vector3& v);
	static Vector3 direction(const Vector3& from, const Vector3& to);
	static const float dotProduct(const Vector3& v1, const Vector3& v2);
	static Vector3 clamp(const Vector3& vec, float min, float max);
	static Vector3 reflect(const Vector3& incident, const Vector3& normal);
	static Vector3 lerp(const Vector3& v1, const Vector3& v2, float lerpFactor);
	static Vector3 crossProduct(const Vector3& v1, const Vector3& v2);
	static float distance(const Vector3& v1, const Vector3& v2);
};

