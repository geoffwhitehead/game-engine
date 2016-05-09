#pragma once
#include "..\engine-base\Entity.h"

class Player;

class Shield :
	public Entity
{
public:

	Shield(string name,
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
		int shield_power,
		Player* owner
	);

	~Shield();
	Player* owner;
	int shield_power;
	int current_power;

	void addBodyToWorld(b2World* world, bool is_dynamic);
	void addFixturesToBody(float radius, bool is_sensor);

};
