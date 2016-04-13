#pragma once
#include "Shape.h"

class Circle : public Shape
{
public:
	Circle(float radius);
	~Circle();
	float getRadius() const{ return this->radius; };
private:
	float radius;
};

