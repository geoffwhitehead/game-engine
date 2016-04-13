#pragma once
#include "../engine-base/SubSystem.h"
#include "../engine-base/Entity.h"
#include "../engine-base/GameManager.h"

using namespace std;

class GameEvents :
	public SubSystem
{
public:
	GameEvents(GameManager* gm);
	~GameEvents();
	enum eEventType { ET_COLLISIONS, ET_SOUND, ET_INPUT, ET_LOGIC };
	enum eLogicEvents { LE_GAMEOVER, LE_PLACED_WHITE, LE_POT_BROWN, LE_POT_RED, LE_POT_YELLOW, LE_POT_WHITE, LE_POT_BLUE, LE_POT_PINK, LE_POT_GREEN, LE_POT_BLACK };
	enum eInputEvents { IE_POT_WHITE, IE_STRIKE_CUE, IE_WHITE_PLACED };
	enum eSoundEvents { SE_GAMEOVER, SE_POT, SE_STRIKE_BALL, SE_STRIKE_CUSHION, SE_POT_WHITE, SE_STRIKE_CUE };
	enum eCollisionEvents { CE_BALL_BALL, CE_BALL_CUSHION, CE_BALL_POCKET };
	
	enum eGameState { GS_PLACE_WHITE, GS_PLAYING};
	enum eBallOn { BO_RED, BO_YELLOW, BO_GREEN, BO_BROWN, BO_BLUE, BO_PINK, BO_BLACK };
	
	eGameState game_state;
	eBallOn ball_on;

	void init();
	void update(float msec);
	void destroy();
	void out_clearEvents(eEventType);
	void in_clearEvents(eEventType);
	void handleCollisionEvents();
	void handleInputEvents();
	void handleLogicEvents();

	Vector3 getMousePos3D();

	GameManager* gm;

	// sorted events - each sub system must handle deletion per frame of their own events
	// collision events
	vector<pair<Entity*, Entity*> > out_cols_circle_circle;
	vector<pair<Entity*, Entity*> > out_cols_circle_cushion;
	vector<pair<Entity*, Entity*> > out_cols_circle_pocket;

	vector<eCollisionEvents> out_collision_events;

	// sound events
	vector<eSoundEvents> in_sound_events;

	// input events
	vector<eInputEvents> in_input_events;
	vector<eInputEvents> out_input_events;

	// logic events
	vector<eLogicEvents> in_logic_events;
	vector<eLogicEvents> out_logic_events;

};

