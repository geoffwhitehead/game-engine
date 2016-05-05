#pragma once
#include "..\engine-base\Entity.h"

/*
*	Class: Player
*	Author:	Geoff Whitehead
*	Description: At the moment only adds one variable: health.
*/


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
		float density
	);
	~Player();

	int resources;
	Entity* selected_node;
	vector <Entity*> nodes;

	void addBodyToWorld(b2World* world, bool is_dynamic);
	void addFixturesToBody(float radius, bool is_sensor);

};

