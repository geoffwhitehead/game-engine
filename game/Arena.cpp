#include "Arena.h"



Arena::Arena(
	string name,
	string str_parent,
	string group,
	string sub_group,
	Vector3 pos,
	Mesh* mesh,
	Shader* shader,
	GLuint texture,
	bool rend,
	bool phys,
	bool dynamic,
	bool sensor,
	float ppm,
	float radius,
	b2World* world,
	float friction,
	float density
) : Entity(name, str_parent, group, sub_group, pos, mesh, shader, texture, rend, phys, ppm, world) {

	addBodyToWorld(world, dynamic);
	addFixturesToBody(radius, sensor);
	b2Fixture* b2_list = this->getPhysicsObject()->body->GetFixtureList();
	b2_list->SetFriction(friction);
	b2_list->SetDensity(density);
	this->physicsObject->body->SetUserData(this);
}


Arena::~Arena() {
}

void Arena::addBodyToWorld(b2World* world, bool is_dynamic) {
	// let superclass to the work, we just need to set the player to be
	// a bullet so it doesn't fall through the world on huge updates
	this->physicsObject->addBodyToWorld(world, is_dynamic);
	this->physicsObject->body->SetActive(false);
	this->getPhysicsObject()->body->SetBullet(true);

}

void Arena::addFixturesToBody(float radius, bool is_sensor) {
	this->getPhysicsObject()->addCircularFixtureToBody(radius, is_sensor);
}
