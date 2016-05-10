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

	
}

Connector::~Connector() {
}