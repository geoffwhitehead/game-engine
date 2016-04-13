#pragma once


class SubSystem
{
public:
	SubSystem();
	virtual ~SubSystem();
	virtual void update(float) = 0;
	virtual void destroy() = 0;
	virtual void init() = 0;

};


