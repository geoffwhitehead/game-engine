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

	Connector* left = nullptr;
	Connector* right = nullptr;

	NodeHub* left_node = nullptr;
	NodeHub* right_node = nullptr;

	time_point<system_clock> created_on;
	Player* owner;

	~Connector();
};

