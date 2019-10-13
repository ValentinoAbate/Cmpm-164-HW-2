#pragma once
#include "Vector3.h"

class Light
{
public: 
	Vector3 position;
	Vector3 color;
	float intensity;
	Light(const Vector3& position, const Vector3& color, float intensity) :
		position(position), color(color), intensity(intensity)
	{

	}
};

