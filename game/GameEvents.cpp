#include "GameEvents.h"



GameEvents::GameEvents(GameManager* gm){
	this->gm = gm;
}


GameEvents::~GameEvents()
{
}

void GameEvents::init() {

}
 
// look through all the events passed from the physics sub system and generate events for the 
// different sub systems to handle
void GameEvents::handleCollisionEvents() {

	for (int i = 0; i < out_cols_circle_circle.size(); i++) {
		in_sound_events.push_back(eSoundEvents::SE_STRIKE_BALL);
	}
	for (int i = 0; i < out_cols_circle_cushion.size(); i++) {
		in_sound_events.push_back(eSoundEvents::SE_STRIKE_CUSHION);
	}
			
	for (int i = 0; i < out_cols_circle_pocket.size(); i++) {
		in_sound_events.push_back(eSoundEvents::SE_POT);
		if (out_cols_circle_pocket[i].first->name == "white") {
			in_logic_events.push_back(eLogicEvents::LE_POT_WHITE);
			in_sound_events.push_back(eSoundEvents::SE_POT_WHITE);
		}
		if (out_cols_circle_pocket[i].first->sub_group == "red") {
			in_logic_events.push_back(eLogicEvents::LE_POT_RED);
		}
		if (out_cols_circle_pocket[i].first->name == "yellow") {
			in_logic_events.push_back(eLogicEvents::LE_POT_YELLOW);
		}
		if (out_cols_circle_pocket[i].first->name == "green") {
			in_logic_events.push_back(eLogicEvents::LE_POT_GREEN);
		}
		if (out_cols_circle_pocket[i].first->name == "blue") {
			in_logic_events.push_back(eLogicEvents::LE_POT_BLUE);
		}
		if (out_cols_circle_pocket[i].first->name == "pink") {
			in_logic_events.push_back(eLogicEvents::LE_POT_PINK);
		}
		if (out_cols_circle_pocket[i].first->name == "brown") {
			in_logic_events.push_back(eLogicEvents::LE_POT_BROWN);
		}
		if (out_cols_circle_pocket[i].first->name == "black") {
			in_logic_events.push_back(eLogicEvents::LE_POT_BLACK);
		}
	}
}

// look through all the events passed from the input sub system and generate events for the 
// different sub systems to handle
void GameEvents::handleInputEvents() {
	for (int i = 0; i < out_input_events.size(); i++) {
		switch (out_input_events[i]) {
		case GameEvents::eInputEvents::IE_STRIKE_CUE:
			in_sound_events.push_back(eSoundEvents::SE_STRIKE_CUE);
			

			break;
		case GameEvents::eInputEvents::IE_WHITE_PLACED:
			in_logic_events.push_back(eLogicEvents::LE_PLACED_WHITE);
			break;
		}
	}
}

// look through all the events passed from the logic sub system and generate events for the 
// different sub systems to handle

void GameEvents::handleLogicEvents() {
	for (int i = 0; i < out_logic_events.size(); i++) {
		switch (out_logic_events[i]) {
		case GameEvents::eLogicEvents::LE_GAMEOVER:
			in_sound_events.push_back(eSoundEvents::SE_GAMEOVER);
		}
	}
}

// the repsonsibility of this function is to handle all the classes [out_*] events
void GameEvents::update(float msec) {
	
	handleCollisionEvents();
	handleInputEvents();
	handleLogicEvents();

}

void GameEvents::destroy() {

}

// in_ events should be cleared at the end of that subsystems frame once they are processed 
void GameEvents::in_clearEvents(eEventType et) {
	switch (et)
	{
	case GameEvents::eEventType::ET_SOUND:
		in_sound_events.clear();
		break;
	case GameEvents::eEventType::ET_INPUT:
		in_input_events.clear();
		break;
	case GameEvents::eEventType::ET_LOGIC:
		in_logic_events.clear();
		break;
	}
}

// out_ events are what are generated within that sub system  and should be cleared at the start of
// that subsystems frame once every other sub system has had a chance to process that event
void GameEvents::out_clearEvents(eEventType et) {
	switch (et)
	{
	case eEventType::ET_COLLISIONS:
		out_cols_circle_circle.clear();
		out_cols_circle_cushion.clear();
		out_cols_circle_pocket.clear();
		out_collision_events.clear();
		break;
	case eEventType::ET_INPUT:
		out_input_events.clear();
		break;
	case eEventType::ET_LOGIC:
		out_logic_events.clear();
		break;
	}
}

// return the 3d mouse position.
Vector3 GameEvents::getMousePos3D() {
	Vector2 pos = gm->getWindow()->GetOSMousePosition();
	Vector2 mPos = gm->getWindow()->convertToScreenCoords(pos);
	return Vector3(mPos.x, mPos.y, 5.0f);
}