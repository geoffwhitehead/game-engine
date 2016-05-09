#include "Connector.h"


Connector::Connector(
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
	Player * owner,
	int cost

) : Node(name, str_parent, group, sub_group, pos, mesh, shader, texture, rend, phys, dynamic, sensor, ppm, radius, world, friction, density, health, owner, cost) {

	addBodyToWorld(world, dynamic);
	addFixturesToBody(radius, sensor);
	b2Fixture* b2_list = this->getPhysicsObject()->body->GetFixtureList();
	b2_list->SetFriction(friction);
	b2_list->SetDensity(density);
	this->physicsObject->body->SetUserData(this);
	this->created_on = system_clock::now();
	this->owner = owner;
}

Connector::~Connector() {
}