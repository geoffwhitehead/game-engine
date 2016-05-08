#include "GameAudio.h"

#define turn_swap "beep1"
#define rocket_1 "rocket1"
#define fart "fart-2"
#define powerup2 "powerup2"
#define powerup3 "powerup3"
#define powerup4 "powerup4"
#define beep1 "beep1"
#define beep2 "beep2"
#define boom "boom"
#define destroyed "destroyed"
#define insuf_resource "beep3"
#define powerdown_resource "powerdown1"
#define charge "blip"
#define charge2 "charge2"
#define chargefull "charge-full"
#define launch "launch"

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
			am->play2D(boom);
			break;
		case GameLogic::eAudioEvents::AE_POWERUP:
			am->play2D(powerup3);
			break;
		case GameLogic::eAudioEvents::AE_HUB_DAMAGED:
			am->play2D(beep2);
			break;
		case GameLogic::eAudioEvents::AE_HUB_DESTROYED:
			am->play2D(destroyed);
			break;
		case GameLogic::eAudioEvents::AE_INSUF_RESOURCE:
			am->play2D(insuf_resource);
			break;
		case GameLogic::eAudioEvents::AE_POWERUP_RESOURCE:
			am->play2D(powerup4);
			break;
		case GameLogic::eAudioEvents::AE_POWERDOWN_RESOURCE:
			am->play2D(powerdown_resource);
			break;
		case GameLogic::eAudioEvents::AE_CHARGE_1:
			am->play2D(charge);
			break;
		case GameLogic::eAudioEvents::AE_CHARGE_2:
			am->play2D(charge2);
			break;
		case GameLogic::eAudioEvents::AE_CHARGE_FULL:
			am->play2D(chargefull);
			break;
		case GameLogic::eAudioEvents::AE_LAUNCH:
			am->play2D(launch);
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