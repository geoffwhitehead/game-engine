#include "PhysicsManager.h"



PhysicsManager::PhysicsManager(b2World* b2_world, float b2_gravity) {
	createWorld(b2_world, b2_gravity);
}

void PhysicsManager::createWorld(b2World* b2_world, float b2_gravity) {
	this->b2_world = new b2World(b2Vec2(0.0f, b2_gravity));
	this->b2_gravity = b2_gravity;
	b2_world->SetAllowSleeping(true);
	b2_world->SetContinuousPhysics(true);
	//b2_world->SetContactListener(this);
}

PhysicsManager::~PhysicsManager() {
}

void update(float) {

}
void destroy() {

}
void init() {

}
void addSubSystem(SubSystem*) {

}

