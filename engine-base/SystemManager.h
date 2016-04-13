#pragma once
#include "SubSystem.h"


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

