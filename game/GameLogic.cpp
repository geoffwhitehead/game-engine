#include "GameLogic.h"

#define VEC_STILL Vector3(0.0,0.0,5.0)



GameLogic::GameLogic(GameManager* gm, GameLogicManager* glm){
	this->glm = glm;
	this->gm = gm;
}



GameLogic::~GameLogic()
{
}

// logic events that occur at the start of the game
void GameLogic::init() {

}

void GameLogic::update(float msec) {
	//ge->out_clearEvents(GameEvents::ET_LOGIC);
	handleEvents();
	handleStates();
	input_events.clear();
}

void GameLogic::destroy() {

}

// adjust whether an entity is rendered or colldable
void GameLogic::editEntity(string name, string parent, bool is_collidable, bool is_renderable) {
	
	Entity* e = gm->getEntityByName(name, parent);
	e->is_renderable = is_renderable;
	e->is_physical = is_collidable;

}

// determine actions based on logic events that have been stacked between frame 
void GameLogic::handleEvents() {
	for (int i = 0; i < input_events.size(); i++) {
		switch (input_events[i]) {
		case LEFT_CLICK:
			cout << "left click" << endl;
			Entity* e = gm->getEntityByName("player_1", "");
			e->getPhysicsObject()->body->ApplyForce(b2Vec2(0.0, 0.1), e->getPhysicsObject()->body->GetWorldCenter(), true);
		}
	}
}

// actions that occur every frame when in a particular state
void GameLogic::handleStates() {
	switch (game_state) {
	case eGameState::PLAYER_1:
		//Entity* white = ge->gm->getEntityByName("white", "table");
		//cout << "player" << endl;
		break;

	}
}

// return the 3d mouse position.
Vector3 GameLogic::getMousePos3D() {
	Vector2 pos = gm->getWindow()->GetOSMousePosition();
	Vector2 mPos = gm->getWindow()->convertToScreenCoords(pos);
	return Vector3(mPos.x, mPos.y, 5.0f);
}