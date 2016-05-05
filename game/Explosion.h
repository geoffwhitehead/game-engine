#pragma once
#include "..\engine-base\Entity.h"

/*
*	Class: Explosion
*	Author:	Geoff Whitehead
*	Description: Class for explosions. Contains vars to determine the length of the explosion
*/


class Explosion : public Entity {

	public:

	Explosion(string name,
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
	);

~Explosion();

int lifetime;
int current_life;
int damage;

void addBodyToWorld(b2World* world, bool is_dynamic);
void addFixturesToBody(float radius, bool is_sensor);

};

