#pragma once
#include "NodeHub.h"

class Shield;

class NodeHubShield :
	public NodeHub
{
public:

	NodeHubShield::NodeHubShield(
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
		int cost

	);

	Shield* shield = 0;

	virtual ~NodeHubShield();
};
