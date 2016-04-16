#pragma once
#include "../engine-io/IOManager.h"
#include "../engine-base/Entity.h"


class CollisionManager;

class GameIO :
	public IOManager {

public:
	GameIO(string root_dir, b2World* b2_world, float ppm);
	~GameIO();
	void loadEntities();
};

