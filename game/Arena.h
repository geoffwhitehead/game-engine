#pragma once
#include "..\engine-base\Entity.h"

/*
*	Class: Arena
*	Author:	Geoff Whitehead
*	Description: Using this class for all quad objects that are related to the game level design and environment.
*/

class Arena :
	public Entity {
public:
	Arena(
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
	~Arena();


	void addBodyToWorld(b2World* world, bool is_dynamic);
	void addFixturesToBody(float radius, bool is_sensor);

};

