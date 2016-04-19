#pragma once
#include "SubSystem.h"

/*
*	Class: SystemManager
*	Author:	Geoff Whitehead
*	Description:  System managers are registered and stored wihtin the game manager. They contain functionality specific 
		to one area. Such as input or audio. The game manager will call the virtual functions at various points 
		during the game
*/

class SystemManager
{
public:
	SystemManager();
	virtual ~SystemManager();
	virtual void update(float) = 0;
	virtual void destroy() = 0;
	virtual void init() = 0;
	virtual void addSubSystem(SubSystem*) = 0;
};

