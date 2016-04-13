#pragma once
#include "../engine-base/SubSystem.h"
#include "../engine-base/GameManager.h"
#include "../nclgl/Window.h"
#include "../engine-base/Camera.h"
#include <assert.h>     /* assert */
#include "GameEvents.h"

#define FORCE_SCALE 120

class GameInput : public SubSystem
{
public:
	GameInput(GameManager* gm, GameEvents* ge, Camera* cam);
	~GameInput();
	void init();
	void update(float msec);
	void destroy();
	void handleControls();
	Vector3 getMousePos3D();
	void shootCueBall();
	void handleMouse();
	void handleEvents();
	bool isLegalPosition();

	GameManager* gm;
	GameEvents* ge;
	Camera * cam;
};

