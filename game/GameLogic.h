#pragma once
#include "../engine-base/GameManager.h"
#include "../engine-base/SubSystem.h"
#include "../engine-base/GameLogicManager.h"
#include "Player.h"
#include "Bomb.h"
#include "Arena.h"
#include "Explosion.h"
#include "../engine-audio/AudioManager.h"
#include "Explosion.h"
#include <math.h>
#include <chrono>
#include <ctime>
#include <iostream>

/*
*	Class: GameLogic
*	Author:	Geoff Whitehead
*	Description: Handles all game evntrs and applies game logic to them. Handles states and 
		functions that occur because of states or events.
*/

using namespace std::chrono;
class GameLogic :
	public SubSystem
{
public:

	enum ePlayerTurn { GS_PLAYER_1, GS_PLAYER_2 };
	enum eGameState { GS_PLAYING, GS_FIRING, GS_BUILDING, GS_EXPLODING, GS_CHARGING };
	enum eInputEvents { IE_LEFT_CLICK, IE_ENTER, IE_SPACE, IE_LEFT, IE_RIGHT};
	enum eAudioEvents { AE_TURN_SWAP, AE_EXPLOSION_BOMB, AE_MOVE };
	enum eGameEvents { GS_QUIT };
	
	eGameState game_state;
	ePlayerTurn player_turn;

	enum eFilter {
		eFilterLevel = 0x01,
		eFilterSolid = 0x02,
		eFilterNonSolid = 0x04
	};

	enum eMask {
		eNoCollide = 0x0000,
		eCollide = 0xffff,
	};

	Entity* active_player;

	
	time_point<system_clock> start;
	time_point<std::chrono::system_clock> end;
	duration<double> elapsed_seconds;

	float charge;
	bool charging;
	int temp = 100;
	GameLogic(GameManager* gm, GameLogicManager* glm, b2World* world, AudioManager* am, Camera* cam);
	~GameLogic();

	void init();
	void update(float msec);
	void destroy();
	static b2Vec2 force;

	void handleEvents();
	void handleStates();
	void endTurn();
	b2Filter createFilter(eFilter filter, eMask mask);
	void setPointer();


	void fireWeapon(float charge);
	bool isAwake(string name, string parent);
	void adjustDirection(eInputEvents);

	b2Filter getFixture(enum eFilter, enum eMask);

	void editEntity(string name, string parent, bool is_collidable, bool is_renderable);
	Vector3 getMousePos3D();

	b2World* world;

	vector<eInputEvents> in_input_events;
	vector<eAudioEvents> out_audio_events;
	vector<pair<Entity*, Entity*>> in_contact_events;
	vector<eGameEvents> in_game_events;
private:
	GameLogicManager* glm;
	GameManager* gm;
	AudioManager* am;
	Camera* cam;
	Entity* p1;
	Entity* p2;
	Entity* pointer;
};

