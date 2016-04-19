#pragma once
#include "../engine-base/SubSystem.h"
#include "../engine-audio/AudioManager.h"
#include "GameLogic.h"

/*
*	Class: GameAudio
*	Author:	Geoff Whitehead
*	Description: Contains information on all audio related events. Each update will do a switch over all the events in its vector
		playing a different sound for each sound event.
*/

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

