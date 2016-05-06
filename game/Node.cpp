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
	Player* owner, 
	int cost

) : LevelEntity(name, str_parent, group, sub_group, pos, mesh, shader, texture, rend, phys, dynamic, sensor, ppm, radius, world, friction, density, cost) {
	
	this->created_on = system_clock::now();
	this->health = health;
	this->owner = owner;
}

Player* owner;
