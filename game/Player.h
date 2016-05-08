#pragma once
#include "LevelEntity.h"

/*
*	Class: Player
*	Author:	Geoff Whitehead
*	Description: At the moment only adds one variable: health.
*/

class Node;
class NodeHubResource;
class NodeHubShield;


class Player :
	public Entity {
public:
	Player(
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
		string colour_mesh
	);
	~Player();

	int total_resource;
	int current_resource;

	LevelEntity* selected_node;

	vector <Node*> nodes;
	vector <NodeHubResource*> resource_nodes;
	vector <NodeHubShield*> shield_nodes;

	string player_mesh;

	void addBodyToWorld(b2World* world, bool is_dynamic);
	void addFixturesToBody(float radius, bool is_sensor);

};

