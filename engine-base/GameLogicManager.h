#pragma once
#include "SystemManager.h"
#include <vector>

/*
*	Class: GameLogicManager
*	Author:	Geoff Whitehead
*	Description: At this stage this class isnt really necessary. A user will instantiate this class and register a logic class 
		with it. It isnt really necessary to do this. Perhaps it will be useful when i want to have seperate logic classes
		per level and can register all of these with this class and then provide some functionality for this in here
*/

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

