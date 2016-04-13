#include "Rectangle.h"


Rectangle::Rectangle(PhysicsObject* obj, float* x, float* y)
{
	this->x = x;
	this->y = y;
	this->physicsObject = obj;
}


Rectangle::~Rectangle()
{
}
