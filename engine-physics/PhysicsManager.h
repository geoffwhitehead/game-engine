#pragma once
#include "../engine-base/SystemManager.h"
#include <Box2D\Box2D.h>
//#include "PhysicsObject.h"
#include "../engine-base/GameManager.h"

/*
*	Class: PhysicsManager
*	Author:	Geoff Whitehead
*	Description: Handles all Box2D related things such as the box 2d world. 
*/

static class PhysicsManager :
	public SystemManager {
public:
	PhysicsManager(GameManager* gm, float b2_gravity, float pixels_per_m);
	~PhysicsManager();

	
	void update(float);
	void destroy();
	void init();
	void addSubSystem(SubSystem*);
	void createWorld( float b2_gravity);
	void updateWorld(float msec);
	b2World* b2_world;
	float pixels_per_m;
	void addListener(b2ContactListener* cl);
	b2ContactListener* contact_listener;

	//in FooTest constructor
	
private:
	float b2_gravity;
	//const float update_interval = 1.0f / 60.0f; // 60hz
	const double sec_per_update = 1000 / 60;
	float accumulator;
	GameManager* gm;
};

