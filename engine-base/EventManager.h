#pragma once
#include "SystemManager.h"
#include <vector>
class EventManager :
	public SystemManager
{
public:
	EventManager();
	~EventManager();
	void update(float);
	void destroy();

	void init();
	void addSubSystem(SubSystem*);

	std::vector<SubSystem*> sub_systems;
};


