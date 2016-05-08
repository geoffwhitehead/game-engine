#include "EnvEntity.h"



EnvEntity::EnvEntity(
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
	float c_x,
	float c_y,
	b2World* world,
	float friction,
	float density

) : Entity(name, str_parent, group, sub_group, pos, mesh, shader, texture, rend, phys, ppm, world) {

	addBodyToWorld(world, dynamic);
	addFixturesToBody(c_x, c_y, sensor);
	b2Fixture* b2_list = this->getPhysicsObject()->body->GetFixtureList();
	b2_list->SetFriction(friction);
	b2_list->SetDensity(density);
	this->physicsObject->body->SetUserData(this);

}

EnvEntity::~EnvEntity() {
}

void EnvEntity::addBodyToWorld(b2World* world, bool is_dynamic) {
	this->physicsObject->addBodyToWorld(world, is_dynamic);
	this->getPhysicsObject()->body->SetBullet(true);

}

void EnvEntity::addFixturesToBody(float width, float height, bool is_sensor) {
	this->getPhysicsObject()->addRectangularFixtureToBody(width, height, is_sensor);
}