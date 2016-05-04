#pragma once
#include "Node.h"
class NodeHub :
	public Node {
public:
	
	NodeHub::NodeHub(
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
		int health

	);

	~NodeHub();
};


