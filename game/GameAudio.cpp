#include "GameAudio.h"

#define SOUND_COLLISION "collision"
#define SOUND_GAMEOVER "game_over"
#define SOUND_BG "bg_music"
#define SOUND_CUSHION1 "hit_cushion_1"
#define SOUND_CUSHION2 "hit_cushion_2"
#define SOUND_CUSHION3 "hit_cushion_3"
#define SOUND_CUSHION4 "hit_cushion_4"
#define SOUND_BALL_STRIKE "ball_strike"
#define SOUND_BALL_POT "ball_pot"
#define SOUND_BALL_POT_M "ball_pot_mech"
#define SOUND_BELL "bell"
#define SOUND_CUE_1 "cue_strike_1"
#define SOUND_CUE_2 "cue_strike_2"

int r;

GameAudio::GameAudio(AudioManager* am, IOManager* iom, GameEvents* ge)
{
	this->am = am;
	this->ge = ge;
	this->iom = iom;
}


GameAudio::~GameAudio()
{
}


void GameAudio::init() {
	
}

void GameAudio::handleStates() {

}

void GameAudio::handleEvents() {

	for (int i = 0; i < ge->in_sound_events.size(); i++) {
		switch (ge->in_sound_events[i]) {
		case GameEvents::eSoundEvents::SE_POT:
			am->play2D(SOUND_BALL_POT);
			break;

		case GameEvents::eSoundEvents::SE_STRIKE_BALL:
			am->play2D(SOUND_BALL_STRIKE);
			break;

		case GameEvents::eSoundEvents::SE_POT_WHITE:
			r = roll(1, 3);

			switch (r) {
			case 1:
				am->play2D(SOUND_CUSHION2);
				break;

			case 2:
				am->play2D(SOUND_CUSHION3);
				break;

			case 3:
				am->play2D(SOUND_CUSHION4);
				break;
			
				
			}
			break;

		case GameEvents::eSoundEvents::SE_GAMEOVER:
			am->play2D(SOUND_GAMEOVER);
			break;

		case GameEvents::eSoundEvents::SE_STRIKE_CUSHION:

			// find a ball hitting cushion sound
			break;

		case GameEvents::eSoundEvents::SE_STRIKE_CUE:
			r = roll(0, 2);
			switch (r) {
			case 0:
				am->play2D(SOUND_CUE_1);
				break;

			case 1:
				am->play2D(SOUND_CUE_2);
				break;

			}

			break;

		}
	}
}

void GameAudio::update(float msec) {

	handleEvents();

	// clear events after processing them 
	ge->in_clearEvents(GameEvents::ET_SOUND);
	
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