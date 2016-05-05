#include "Node.h"


Node::~Node() {

}

Node::Node(
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
	float density,
	float health,
	Player* owner

) : Entity(name, str_parent, group, sub_group, pos, mesh, shader, texture, rend, phys, ppm, world) {
	
	this->created_on = system_clock::now();
	this->health = health;
	this->owner = owner;
	addBodyToWorld(world, dynamic);
	addFixturesToBody(radius, sensor);
	b2Fixture* b2_list = this->getPhysicsObject()->body->GetFixtureList();
	b2_list->SetFriction(friction);
	b2_list->SetDensity(density);
	this->physicsObject->body->SetUserData(this);
}

Player* owner;

void Node::addBodyToWorld(b2World* world, bool is_dynamic) {
	this->physicsObject->addBodyToWorld(world, is_dynamic);
	this->getPhysicsObject()->body->SetBullet(true);

}

void Node::addFixturesToBody(float radius, bool is_sensor) {
	this->getPhysicsObject()->addCircularFixtureToBody(radius, is_sensor);
}
