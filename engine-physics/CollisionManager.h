#pragma once
#include <vector>
#include "../engine-base/SystemManager.h"
#include "../engine-base/SubSystem.h"
#include "../engine-base/Entity.h"
#include <algorithm>
#include "Circle.h"
#include "Rectangle.h"
#include "Plane.h"
#include <map>

class CollisionManager :
	public SystemManager

{
public:
	CollisionManager();
	~CollisionManager();

	void init();
	void update(float msec);
	void destroy();
	void addSubSystem(SubSystem*);
	
	void addObject(Entity* phys_obj, float radius);
	void addObject(Entity* entity, float distance, Vector3 normal);
	void removeEntity(Entity* e);
	void manageCollisions(float msec);
	void clearFrame();

	vector<SubSystem*> sub_systems;
	vector<pair<Entity*, Entity*> > collisions_this_frame;

private:
	vector<Entity*> collidableSpheres;
	vector<Entity*> collidablePlanes;
	std::map<Entity*, std::map<Entity*, bool>> collision_map;
	
};
