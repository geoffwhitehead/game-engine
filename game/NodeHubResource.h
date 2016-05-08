#pragma once
#include "NodeHub.h"
class NodeHubResource :
	public NodeHub
{
public:

	NodeHubResource::NodeHubResource(
		std::string name,
		std::string str_parent,
		std::string group,
		std::string sub_group,
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
		int health,
		Player* owner,
		int cost,
		int resource_per_turn

	);

	bool is_powered_on = false;
	int resource_per_turn;

	virtual ~NodeHubResource();
};

