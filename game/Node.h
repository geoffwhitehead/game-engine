#pragma once
#include "LevelEntity.h"
#include "Player.h"
#include <chrono>
#include <ctime>


using namespace std::chrono;

class Node :
	public LevelEntity {

public:
	Node(
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

	virtual ~Node();
	float health;


	time_point<system_clock> created_on;
	Player* owner;

};