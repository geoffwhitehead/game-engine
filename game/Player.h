#pragma once
#include "C:\Users\Geoff\Source\Repos\game-engine\engine-base\Entity.h"
class Player :
	public Entity {
public:
	Player(string name, string str_parent, string group, string sub_group, Vector3 pos, Mesh* mesh, Shader* shader, GLuint texture, bool rend, bool phys, float ppm, float rad, b2World* world, float friction, float density);
	~Player();


	void addBodyToWorld(b2World* world);
	void addFixturesToBody(float radius);

};

