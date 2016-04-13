#pragma once
#include "SystemManager.h"
#include <vector>

class GameLogicManager :
	public SystemManager
{
public:
	GameLogicManager();
	~GameLogicManager();

	void update(float);
	void destroy();
	void init();
	void addSubSystem(SubSystem*);

	std::vector<SubSystem*> sub_systems;

};

