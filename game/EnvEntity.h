#pragma once
#include "..\engine-base\Entity.h"
class EnvEntity :
	public Entity
{
public:
	EnvEntity(
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
		float pixels_per_m,
		float c_x,
		float c_y,
		b2World* world,
		float friction,
		float density
	);



	void addBodyToWorld(b2World* world, bool is_dynamic);
	void addFixturesToBody(float width, float height, bool is_sensor);


	virtual ~EnvEntity();
};

