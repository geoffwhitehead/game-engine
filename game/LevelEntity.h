#pragma once
#include "..\engine-base\Entity.h"
class LevelEntity :
	public Entity
{
public:
	LevelEntity(
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
		int cost
	);



	void addBodyToWorld(b2World* world, bool is_dynamic);
	void addFixturesToBody(float radius, bool is_sensor);


	int cost;
	virtual ~LevelEntity();
};

