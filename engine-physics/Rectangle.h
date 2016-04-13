#pragma once

#include "Shape.h"
#include "../engine-base/PhysicsObject.h"

class Rectangle : public Shape
{
public:
	Rectangle(PhysicsObject* obj, float* x, float* y);
	~Rectangle();
private:
	float* x;
	float* y;
	PhysicsObject* physicsObject;
};

