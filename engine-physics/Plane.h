#pragma once
#include "Shape.h"
#include "../engine-base/PhysicsObject.h"
class Plane :
	public Shape
{
public:
	Plane(float distance, Vector3 normal);
	~Plane();
	Vector3 getNormal();
	float Plane::getDistanceFromOrigin();
private:
	float distance;
	Vector3 normal;
};