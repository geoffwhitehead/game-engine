#include "GameInput.h"

bool last_frame_mleft_pressed = false;
bool enter_pressed_last_frame = false;

GameInput::GameInput(GameLogic* gl,  Camera * cam)
{
	this->gl = gl;
	this->cam = cam;
}

void GameInput::init() {

}

GameInput::~GameInput(){

}

void GameInput::update(float msec) {
	
	gl->in_input_events.clear();

	handleControls();
	handleEvents();
}

void GameInput::destroy() {

}

// currently just handles mouse, add keyboard or other devices here
void GameInput::handleControls() {
	handleMouse();
	handleKeyboard();
}

// handle any events that affect how the controls are used
void GameInput::handleEvents() {
	/*for (int i = 0; i < ge->in_input_events.size(); i++) {
		switch (ge->in_input_events[i]) {
		case GameEvents::IE_POT_WHITE:
			break;
		}
	}*/
}

void GameInput::handleKeyboard() {
	
	if (Keyboard::KeyTriggered(KeyboardKeys(KEY_RETURN))) {
		gl->in_input_events.push_back(GameLogic::eInputEvents::IE_ENTER);
	}
}

Vector3 GameInput::getMousePos3D() {
	Vector2 pos = gm->getWindow()->GetOSMousePosition();
	Vector2 mPos = gm->getWindow()->convertToScreenCoords(pos);
	return Vector3(mPos.x, mPos.y, 5.0f);
}


// handle mouse buttons
void GameInput::handleMouse() {
	bool mouse_left_button_down = Mouse::ButtonDown(MouseButtons(MOUSE_LEFT));
	
	if (mouse_left_button_down && !last_frame_mleft_pressed) {

		gl->in_input_events.push_back(GameLogic::eInputEvents::IE_LEFT_CLICK);
		last_frame_mleft_pressed = true;
	}
	else if (!mouse_left_button_down) {
		last_frame_mleft_pressed = false;
	}
	

	
	if (mouse_left_button_down && !last_frame_mleft_pressed) {
			
		//gl->input_events.push_back(GameLogic::PLAYER_2);
		last_frame_mleft_pressed = true;
	}
	else if (!mouse_left_button_down) {
		last_frame_mleft_pressed = false;
	}
	
}

