#include "Resource.h"



Resource::Resource(
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

) : EnvEntity(name, str_parent, group, sub_group, pos, mesh, shader, texture, rend, phys, dynamic, sensor, ppm, c_x, c_y, world, friction, density) {
}


Resource::~Resource() {

}