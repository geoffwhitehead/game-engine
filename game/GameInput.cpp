#include "GameInput.h"

bool white_needs_placing = true;
bool last_frame_mleft_pressed = false;

GameInput::GameInput(GameManager* gm, GameEvents* ge, Camera * cam)
{
	this->gm = gm;
	this->cam = cam;
	this->ge = ge;
}

void GameInput::init() {

}

GameInput::~GameInput()
{
}

void GameInput::update(float msec) {
	
	ge->out_clearEvents(GameEvents::ET_INPUT);

	handleControls();
	handleEvents();

	ge->in_clearEvents(GameEvents::ET_INPUT);
}

void GameInput::destroy() {

}

// currently just handles mouse, add keyboard or other devices here
void GameInput::handleControls() {
	handleMouse();
}

// handle any events that affect how the controls are used
void GameInput::handleEvents() {
	for (int i = 0; i < ge->in_input_events.size(); i++) {
		switch (ge->in_input_events[i]) {
		case GameEvents::IE_POT_WHITE:
			break;
		}
	}
}

Vector3 GameInput::getMousePos3D() {
	Vector2 pos = gm->getWindow()->GetOSMousePosition();
	Vector2 mPos = gm->getWindow()->convertToScreenCoords(pos);
	return Vector3(mPos.x, mPos.y, 5.0f);
}

// apply a vector to the cue ball based on how far from the cue ball you click, and in the
// direction of the click
void GameInput::shootCueBall() {
	Entity* white = gm->getEntityByName("white", "table");
	Vector3 cueBallPos = white->getPhysicsObject()->getPos();

	// apply vector to cue ball
	Vector3 dir = getMousePos3D() - cueBallPos;
	white->setVel(dir / FORCE_SCALE);
}

// handle mouse buttons
void GameInput::handleMouse() {
	bool mouse_left_button_down = Mouse::ButtonDown(MouseButtons(MOUSE_LEFT));

	if (ge->game_state == GameEvents::eGameState::GS_PLACE_WHITE){
		if (mouse_left_button_down && !last_frame_mleft_pressed) {

			ge->out_input_events.push_back(GameEvents::IE_WHITE_PLACED);
			last_frame_mleft_pressed = true;
		}
		else if (!mouse_left_button_down) {
			last_frame_mleft_pressed = false;
		}
	}

	if (ge->game_state == GameEvents::eGameState::GS_PLAYING) {
		if (mouse_left_button_down && !last_frame_mleft_pressed) {
			shootCueBall();
			ge->out_input_events.push_back(GameEvents::IE_STRIKE_CUE);
			last_frame_mleft_pressed = true;
		}
		else if (!mouse_left_button_down) {
			last_frame_mleft_pressed = false;
		}
	}
}

