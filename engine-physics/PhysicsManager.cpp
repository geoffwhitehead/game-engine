#include "PhysicsManager.h"
#include <assert.h>


PhysicsManager::PhysicsManager(GameManager* gm, float b2_gravity, float pixels_per_m) {
	assert(b2_gravity != 0 || pixels_per_m != 0);
	createWorld(b2_gravity);
	this->pixels_per_m = pixels_per_m;
	this->gm = gm;
}

void PhysicsManager::createWorld(float b2_gravity) {
	this->b2_world = new b2World(b2Vec2(0.0f, b2_gravity));
	this->b2_gravity = b2_gravity;
	b2_world->SetAllowSleeping(true);
	b2_world->SetContinuousPhysics(true);
	
}

PhysicsManager::~PhysicsManager() {

}

void PhysicsManager::update(float msec) {
	
	updateWorld(msec);

}

void PhysicsManager::addListener(b2ContactListener* cl) {
	this->contact_listener = cl;
	b2_world->SetContactListener(contact_listener);
}


void PhysicsManager::destroy() {

}
void PhysicsManager::init() {

}
void PhysicsManager::addSubSystem(SubSystem*) {

}

void PhysicsManager::updateWorld(float msec) {
	// determine the amount of time elapsed since our last update
	double frameTime = msec;
	accumulator += msec;

	// update the world with the same seconds per update
	while (accumulator > sec_per_update) {
		accumulator -= sec_per_update;

		// perform a single step of the physics simulation
		b2_world->Step(sec_per_update, 8, 1);
	}

}

