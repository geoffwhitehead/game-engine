#pragma once
#include "../engine-base/SubSystem.h"
#include "../engine-base/SystemManager.h"
#include <vector>

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

