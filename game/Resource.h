#pragma once
#include "EnvEntity.h"
class Resource :
	public EnvEntity
{
public:
	Resource(string name,
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
		float pixels_per_m,
		float c_x,
		float c_y,
		b2World* world,
		float friction,
		float density
	);

	virtual ~Resource();


};
