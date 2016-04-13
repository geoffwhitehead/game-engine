#include "Plane.h"


Plane::Plane(float distance, Vector3 normal)
{
	this->distance = distance;
	this->normal = normal;
	//this->normal = physicsObject->getPos();
	//this->normal.Normalise();
}


Plane::~Plane()
{
}

Vector3 Plane::getNormal(){
	return normal;
}

float Plane::getDistanceFromOrigin(){
	return distance;
}
