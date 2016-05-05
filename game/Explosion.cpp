#include "Explosion.h"


Explosion::Explosion(
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
	bool is_dynamic,
	bool is_sensor,
	float ppm,
	float radius,
	b2World* world,
	float friction,
	float density,
	int lifetime,
	int damage
) : Entity(name, str_parent, group, sub_group, pos, mesh, shader, texture, rend, phys, ppm, world) {
	this->damage = damage;
	this->lifetime = lifetime;
	this->current_life = lifetime;
	addBodyToWorld(world, is_dynamic);
	addFixturesToBody(radius, is_sensor);
	b2Fixture* b2_list = this->getPhysicsObject()->body->GetFixtureList();
	b2_list->SetFriction(friction);
	b2_list->SetDensity(density);
	this->physicsObject->body->SetUserData(this);
}


Explosion::~Explosion() {
}

void Explosion::addBodyToWorld(b2World* world, bool is_dynamic) {
	this->physicsObject->addBodyToWorld(world, is_dynamic);

	// explosions are transformed to their point of explosion, must disable sleeping to allow collisions to occur
	this->getPhysicsObject()->body->SetSleepingAllowed(false); 

	this->getPhysicsObject()->body->SetBullet(true);
}

void Explosion::addFixturesToBody(float radius, bool is_sensor) {
	this->getPhysicsObject()->addCircularFixtureToBody(radius, is_sensor);
}
