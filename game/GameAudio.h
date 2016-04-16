#pragma once
#include "../engine-base/SubSystem.h"
#include "../engine-audio/AudioManager.h"
#include "GameEvents.h"
#include "GameLogic.h"

class GameAudio :
	public SubSystem
{
public:
	GameAudio(GameLogic* gl, AudioManager* am);
	~GameAudio();

	void init();
	void update(float msec);
	void destroy();

	int roll(int min, int max);
	void handleEvents();
	void handleStates();

	GameLogic* gl;
	AudioManager* am;
private:
};

