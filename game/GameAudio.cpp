#include "GameAudio.h"

#define turn_swap "swap"
#define rocket_1 "rocket1"
#define fart "fart-2"
#define powerup "powerup"

int r;

GameAudio::GameAudio(GameLogic* gl, AudioManager* am)
{
	this->gl = gl;
	this->am = am;
}


GameAudio::~GameAudio()
{
}


void GameAudio::init() {
	
}

void GameAudio::handleStates() {

}

void GameAudio::handleEvents() {

	for (int i = 0; i < gl->out_audio_events.size(); i++) {
		switch (gl->out_audio_events[i]) {
		case GameLogic::eAudioEvents::AE_TURN_SWAP:
			am->play2D(turn_swap);
			break;
		case GameLogic::eAudioEvents::AE_EXPLOSION_BOMB:
			am->play2D(rocket_1);
			break;
		case GameLogic::eAudioEvents::AE_MOVE:
			am->play2D(fart);
			break;
		case GameLogic::eAudioEvents::AE_POWERUP:
			am->play2D(powerup);
			break;
		}
	}
	
}

void GameAudio::update(float msec) {
	handleEvents();

	gl->out_audio_events.clear();
}

void GameAudio::destroy() {

}

// get a random number - used when determing which random sound to play
int GameAudio::roll(int min, int max)
{
	double x = rand() / static_cast<double>(RAND_MAX + 1);

	int r = min + static_cast<int>(x * (max - min));

	return r;
}