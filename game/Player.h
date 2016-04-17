#pragma once
#include "..\engine-base\Entity.h"
class Player :
	public Entity {
public:
	Player(
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
		int health
	);
	~Player();

	int health;

	void addBodyToWorld(b2World* world, bool is_dynamic);
	void addFixturesToBody(float radius, bool is_sensor);

};

