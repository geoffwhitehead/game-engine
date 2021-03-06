#pragma once
#include "../engine-base/SubSystem.h"
#include "../engine-base/GameManager.h"
#include "GameLogic.h"
#include "../nclgl/Window.h"
#include "../engine-base/Camera.h"
#include <assert.h>     



/*
*	Class: GameInput
*	Author:	Geoff Whitehead
*	Description: Checks for any user input and inserts the event into a vector contained in the game logic class
*/



#define FORCE_SCALE 120

class GameInput : public SubSystem
{
public:
	GameInput(GameLogic* gl, Camera* cam);
	~GameInput();
	void init();
	void update(float msec);
	void destroy();
	void handleControls();
	Vector3 getMousePos3D();
	void handleMouse();
	void handleEvents();
	void handleKeyboard();

	GameManager* gm;
	Camera * cam;
	GameLogic* gl;
};




