#pragma once
#include "../engine-base/SystemManager.h"
#include <Box2D/Box2D.h>

static class PhysicsManager :
	public SystemManager {
public:
	PhysicsManager(b2World* world, float b2_gravity);
	~PhysicsManager();

	void update(float) = 0;
	void destroy() = 0;
	void init() = 0;
	void addSubSystem(SubSystem*) = 0;
	void createWorld(b2World* b2_world, float b2_gravity);

private:
	b2World* b2_world;
	float b2_gravity;
};

