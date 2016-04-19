#pragma once
#include "..\engine-base\Entity.h"


/*
*	Class: Bomb
*	Author:	Geoff Whitehead
*	Description: Class to hold informaiton on a bomb. Add a damage float.
*/

class Bomb :
	public Entity {
public:
	Bomb(string name, 
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
		float damage
	);
	
	~Bomb();


	float damage;


	void addBodyToWorld(b2World* world, bool is_dynamic);
	void addFixturesToBody(float radius, bool is_sensor);

};

