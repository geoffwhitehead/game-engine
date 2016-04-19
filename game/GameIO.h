#pragma once
#include "../engine-io/IOManager.h"
#include "../engine-base/Entity.h"

/*
*	Class: GameIO
*	Author:	Geoff Whitehead
*	Description: Handles the load entity virtual function from the base class. Determines how to laod entities into the game.
*/

class CollisionManager;

class GameIO :
	public IOManager {

public:
	GameIO(string root_dir, b2World* b2_world, float ppm);
	~GameIO();
	void loadEntities();
};

