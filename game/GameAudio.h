#pragma once
#include "../engine-base/SubSystem.h"
#include "../engine-audio/AudioManager.h"
#include "GameEvents.h"

class GameAudio :
	public SubSystem
{
public:
	GameAudio(AudioManager* am, IOManager* iom, GameEvents* ge);
	~GameAudio();

	void init();
	void update(float msec);
	void destroy();

	int roll(int min, int max);
	void handleEvents();
	void handleStates();

private:
	AudioManager* am;
	GameEvents* ge;
	IOManager* iom;
};

