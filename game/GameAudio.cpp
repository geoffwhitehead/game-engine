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
#define select "switch"
#define shield_on "shield_on"
#define shield_off "shield_off"
#define explosion1 "ex1"
#define explosion2 "ex2"
#define explosion3 "ex3"
#define explosion4 "ex4"

int r;
int min = 1;
int max = 4;

std::random_device rd;     // only used once to initialise (seed) engine
std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
std::uniform_int_distribution<int> uni(1, 4); 

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
	int random_integer;
	for (int i = 0; i < gl->out_audio_events.size(); i++) {

		switch (gl->out_audio_events[i]) {
		case GameLogic::eAudioEvents::AE_TURN_SWAP:
			am->play2D(turn_swap);
			break;
		case GameLogic::eAudioEvents::AE_EXPLOSION_BOMB:
			
			random_integer = uni(rng);
			switch (random_integer) {
			case 1:
				am->play2D(explosion1);
				break;
			case 2:
				am->play2D(explosion2);
				break;
			case 3:
				am->play2D(explosion3);
				break;
			case 4:
				am->play2D(explosion4);
				break;
			}
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
		case GameLogic::eAudioEvents::AE_SELECT:
			am->play2D(select);
			break;
		case GameLogic::eAudioEvents::AE_SHIELD_ON:
			am->play2D(shield_on);
			break;
		case GameLogic::eAudioEvents::AE_SHIELD_OFF:
			am->play2D(shield_off);
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
