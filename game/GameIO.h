#pragma once
#include "../engine-io/IOManager.h"
#include "../engine-base/Entity.h"
//#include "../engine-physics/CollisionManager.h"

class CollisionManager;

class GameIO :
	public IOManager {

public:
	GameIO(CollisionManager* cm, string root_dir);
	~GameIO();
	void loadEntities();

	CollisionManager* cm;
};

