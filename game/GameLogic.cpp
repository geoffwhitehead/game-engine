#include "GameLogic.h"

#define VEC_STILL Vector3(0.0,0.0,5.0)
#define CAM_OFFSET 20.0f


GameLogic::GameLogic(GameManager* gm, GameLogicManager* glm, b2World* world, AudioManager* am, Camera* cam){
	this->glm = glm;
	this->gm = gm;
	this->world = world;
	this->am = am;
	this->cam = cam;
}



GameLogic::~GameLogic()
{
}

// logic events that occur at the start of the game
void GameLogic::init() {
	player_turn = ePlayerTurn::GS_PLAYER_1;
	game_state = eGameState::GS_PLAYING;

	Entity* player_1 = gm->getEntityByName("player_1", "");
	player_1->getPhysicsObject()->body->SetLinearDamping(0.0012);

	Entity* player_2 = gm->getEntityByName("player_2", "");
	player_2->getPhysicsObject()->body->SetLinearDamping(0.0005);
}

void GameLogic::update(float msec) {
	
	handleEvents();
	handleStates();
	checkContacts();

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
	for (int i = 0; i < in_input_events.size(); i++) {
		switch (in_input_events[i]) {
			Entity* e;
		case eInputEvents::IE_LEFT_CLICK:
			cout << "left click" << endl;
			e = gm->getEntityByName("player_1", "");
			e->getPhysicsObject()->body->ApplyForce(b2Vec2(0.0, 1.0), e->getPhysicsObject()->body->GetWorldCenter(), true);
			out_audio_events.push_back(eAudioEvents::AE_TURN_SWAP);
			break;

		case eInputEvents::IE_ENTER:
			if (game_state == eGameState::GS_PLAYING) {
				endTurn();
			}
			break;

		}
	}
}


void GameLogic::endTurn() {
	
	if (player_turn == ePlayerTurn::GS_PLAYER_1) {
		player_turn = ePlayerTurn::GS_PLAYER_2;
		Vector3 player_pos = gm->getEntityByName("player_2", "")->getPhysicsObject()->getPos();
		cam->SetPosition(Vector3(player_pos.x, player_pos.y, CAM_OFFSET));

	}
	else {
		player_turn = ePlayerTurn::GS_PLAYER_1;
		Vector3 player_pos = gm->getEntityByName("player_1", "")->getPhysicsObject()->getPos();
		cam->SetPosition(Vector3(player_pos.x, player_pos.y, CAM_OFFSET));
	}
}
// actions that occur every frame when in a particular state
void GameLogic::handleStates() {
	switch (game_state) {
	case eGameState::GS_PLAYING:
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

void GameLogic::checkContacts() {
	for (b2Contact* contact = world->GetContactList(); contact; contact = contact->GetNext()) {
		//check if fixture A was a ball
		void* bodyUserData = contact->GetFixtureA()->GetBody()->GetUserData();
		if (bodyUserData) {
			if (static_cast<Player*>(bodyUserData)->name == "player_1") {
				//cout << "player 1 collided" << endl;
			}
		}
			//static_cast<Player*>(bodyUserData)->startContact();

		//check if fixture B was a ball
		bodyUserData = contact->GetFixtureB()->GetBody()->GetUserData();
		if (bodyUserData) {
			if (static_cast<Player*>(bodyUserData)->name == "player_2") {
				//cout << "player 1 collided with me" << endl;
			}
			
		}
			//static_cast<Player*>(bodyUserData)->startContact();
	}
		
}