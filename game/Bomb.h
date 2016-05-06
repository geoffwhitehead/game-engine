#pragma once

#include "LevelEntity.h"


/*
*	Class: Bomb
*	Author:	Geoff Whitehead
*	Description: Class to hold informaiton on a bomb. 
*/

class Bomb :
	public LevelEntity {
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
		int cost
	);
	
	virtual ~Bomb();


};

