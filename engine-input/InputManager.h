#pragma once
#include "../engine-base/SubSystem.h"
#include "../engine-base/SystemManager.h"
#include <vector>


/*
*	Class: InputManager
*	Author:	Geoff Whitehead
*	Description: Handles input related tasks. At this stage this is nothing more than containing a vector of 
		sub systems that contain input related game specific actions.
*/


using namespace std;

class InputManager :
	public SystemManager
{
public:
	

	InputManager();
	~InputManager();
	void update(float);
	void destroy();
	void init();
	void addSubSystem(SubSystem*);

	vector<SubSystem*> sub_systems;
};

