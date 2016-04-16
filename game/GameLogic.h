#pragma once
#include "../engine-base/SubSystem.h"
#include "../engine-base/GameLogicManager.h"
#include "GameEvents.h"

class GameLogic :
	public SubSystem
{
public:

	enum eGameState { PLAYER_1, PLAYER_2 };
	enum eInputEvents { LEFT_CLICK };

	eGameState game_state;

	vector<eInputEvents> input_events;

	GameLogic(GameManager* gm, GameLogicManager* glm);
	~GameLogic();

	void init();
	void update(float msec);
	void destroy();

	void handleEvents();
	void handleStates();

	void editEntity(string name, string parent, bool is_collidable, bool is_renderable);
	Vector3 getMousePos3D();
private:
	GameLogicManager* glm;
	GameManager* gm;
};

