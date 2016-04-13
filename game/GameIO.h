#pragma once
#include "../engine-io/IOManager.h"
#include "../engine-base/Entity.h"

class CollisionManager;

class GameIO :
	public IOManager {

public:
	GameIO(string root_dir);
	~GameIO();
	void loadEntities();
};

