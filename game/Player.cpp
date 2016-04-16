#include "Player.h"



Player::Player (string name, string str_parent, string group, string sub_group, Vector3 pos, Mesh* mesh, Shader* shader, GLuint texture, bool rend, bool phys, float ppm, float radius, b2World* world, float friction, float density) :
Entity(name, str_parent, group, sub_group, pos, mesh, shader, texture, rend, phys, ppm, world){
	addBodyToWorld(world);
	addFixturesToBody(radius);
	b2Fixture* b2_list = this->getPhysicsObject()->body->GetFixtureList();
	b2_list->SetFriction(friction);
	b2_list->SetDensity(density);
	this->physicsObject->body->SetUserData(this);
}


Player::~Player() {
}

void Player::addBodyToWorld(b2World* world) {
	// let superclass to the work, we just need to set the player to be
	// a bullet so it doesn't fall through the world on huge updates
	this->physicsObject->addBodyToWorld(world);
	this->getPhysicsObject()->body->SetBullet(true);
	
}

void Player::addFixturesToBody(float radius) {
	this->getPhysicsObject()->addCircularFixtureToBody(radius);
}