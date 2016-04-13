#include "PhysicsObject.h"


PhysicsObject::PhysicsObject(Vector3 pos, Vector3 acc, Vector3 vel, float mass, PhysicsObject* parent){

	this->position = pos;
	this->acceleration = acc;
	this->velocity = vel;
	this->renderObject = nullptr;
	this->parent = parent;
	this->mass = mass;
}
PhysicsObject::PhysicsObject(Vector3 pos, Vector3 acc, Vector3 vel, RenderObject *r, float mass, PhysicsObject* parent){
	this->position = pos;
	this->acceleration = acc;
	this->velocity = vel;
	this->renderObject = r;
	this->parent = parent;
	this->mass = mass;
	updateRenderObject();
}
PhysicsObject::~PhysicsObject(){
	
}

Vector3& PhysicsObject::getPos() { return position;}
Vector3& PhysicsObject::getAcc() { return acceleration;}
Vector3& PhysicsObject::getVel() { return velocity;}
float& PhysicsObject::getMass() { return mass; }

void PhysicsObject::setAcc(Vector3 dir){ this->acceleration = dir; };
void PhysicsObject::setVel(Vector3 vel){ this->velocity = vel; };
void PhysicsObject::setMass(float mass){ this->mass = mass; };
void PhysicsObject::setPos(Vector3 pos){
	this->position = pos;
	if (renderObject) { updateRenderObject(); };
};

void PhysicsObject::update(float dt) {
	Physics::semiImplicitEuler(position, velocity, acceleration, prev_displacement, dt);
	updateRenderObject();
};

