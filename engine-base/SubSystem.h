#pragma once

/*
*	Class: SubSystem
*	Author:	Geoff Whitehead
*	Description: Sub systems are registered with System Managers that extend functionality of the 
		system manager in a game specific context. Contains functions that a system manager will call
		that a sub system must implement.
*/

class SubSystem
{
public:
	SubSystem();
	virtual ~SubSystem();
	virtual void update(float) = 0;
	virtual void destroy() = 0;
	virtual void init() = 0;

};


