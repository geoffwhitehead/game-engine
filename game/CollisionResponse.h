#pragma once
#include "../engine-base/SubSystem.h"
#include "../engine-physics/CollisionManager.h"
#include "GameEvents.h"

class CollisionResponse :
	public SubSystem
{
public:
	CollisionResponse(CollisionManager* cm, GameEvents* ge);
	~CollisionResponse();
	void init();
	void update(float msec);
	void destroy();
	void handleCollisions();

	CollisionManager* cm;
	GameEvents* ge;
	
	

};

