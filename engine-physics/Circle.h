#pragma once
#include "Shape.h"
#include "../engine-base/PhysicsObject.h"

class Circle : public Shape
{
public:
	Circle(float radius);
	~Circle();
	float getRadius() const{ return this->radius; };
private:
	float radius;
};

