#pragma once
#include "Node.h"

class NodeHub;

class Connector :
	public Node
{
public:
	Connector(
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
		float health,
		Player * owner,
		int cost

	);

	Connector* left = 0;
	Connector* right = 0;

	NodeHub* left_node = 0;
	NodeHub* right_node = 0;

	time_point<system_clock> created_on;
	Player* owner;

	virtual ~Connector();
};

