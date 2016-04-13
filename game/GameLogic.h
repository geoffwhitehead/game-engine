#pragma once
#include "../engine-base/SubSystem.h"
#include "../engine-base/GameLogicManager.h"
#include "GameEvents.h"

class GameLogic :
	public SubSystem
{
public:
	GameLogic(GameLogicManager* glm, GameEvents* ge);
	~GameLogic();

	void init();
	void update(float msec);
	void destroy();

	void handleEvents();
	void handleStates();

	bool isLegalPosition();
	void resolvePot(string name);
	void editEntity(string name, string parent, bool is_collidable, bool is_renderable);
	void resetBall(string name, Vector3 pos);

private:
	GameLogicManager* glm;
	GameEvents* ge;
};

