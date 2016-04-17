#pragma once
#include "..\_resources\Box2D\Box2D\Dynamics\b2WorldCallbacks.h"
#include "GameLogic.h"
class ContactListener :
	public b2ContactListener {
public:
	ContactListener(GameLogic* gl);
	~ContactListener();
	
	virtual void BeginContact(b2Contact* contact);

	// Called when two fixtures cease to touch
	virtual void EndContact(b2Contact* contact);

	GameLogic* gl;

};

