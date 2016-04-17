#include "GameLogic.h"
#include "Explosion.h"

#define VEC_STILL Vector3(0.0,0.0,5.0)
#define CAM_OFFSET 20.0f
#define DAMPING 0.0005f
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

	gm->getEntityByName("player_1", "")->getPhysicsObject()->body->SetLinearDamping(DAMPING);
	gm->getEntityByName("player_2", "")->getPhysicsObject()->body->SetLinearDamping(DAMPING);
	gm->getEntityByName("bomb", "")->getPhysicsObject()->body->SetLinearDamping(DAMPING);
	gm->getEntityByName("bomb", "")->getPhysicsObject()->body->SetActive(false);
	gm->getEntityByName("bomb_explosion", "")->getPhysicsObject()->body->SetActive(false);
}

void GameLogic::update(float msec) {
	
	handleEvents();
	handleStates();

}


b2Filter GameLogic::getFixture(enum eFilter f, enum eMask m) {
	b2Filter filter;
	filter.categoryBits = f;
	filter.maskBits = m;
	return filter;
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


	// handle input events
	for (int i = 0; i < in_input_events.size(); i++) {
		switch (in_input_events[i]) {
			Entity* e;
			if (game_state == eGameState::GS_PLAYING) {


				case eInputEvents::IE_LEFT_CLICK:
					cout << "left click" << endl;
					e = gm->getEntityByName("player_1", "");
					e->getPhysicsObject()->body->ApplyForce(b2Vec2(0.1, 0.01), e->getPhysicsObject()->body->GetWorldCenter(), true);
					out_audio_events.push_back(eAudioEvents::AE_TURN_SWAP);
					break;

				case eInputEvents::IE_ENTER:
					if (game_state == eGameState::GS_PLAYING) {
						out_audio_events.push_back(eAudioEvents::AE_TURN_SWAP);
						endTurn();

					}
					break;

				case eInputEvents::IE_SPACE:
					if (game_state == eGameState::GS_PLAYING) {
						game_state = eGameState::GS_FIRING;
						fireWeapon();
					}

			}
		}
	}

	// handle in game events
	for (int i = 0; i < in_game_events.size(); i++) {
		switch (in_game_events[i]) {
		case eGameEvents::GS_QUIT:
			gm->getWindow()->forceQuit = true;
		}
	}
}

void GameLogic::fireWeapon() {
	Entity* bomb = gm->getEntityByName("bomb", "");
	bomb->is_renderable = true;
	bomb->getPhysicsObject()->body->SetActive(true);
	bomb->getPhysicsObject()->body->GetFixtureList()->SetFilterData(getFixture(eFilterSolid, eCollide));
	bomb->getPhysicsObject()->body->ApplyForce(b2Vec2(-0.1, 0.001), bomb->getPhysicsObject()->body->GetWorldCenter(), true);
	
	

	
}
/*
void PlayerEntity::boost() {
	float magnitude = 0.1f;
	b2Vec2 force = b2Vec2(cos(m_PhysicsObject->GetAngle()) * magnitude, sin(m_PhysicsObject->GetAngle()) * magnitude);
	m_PhysicsObject->ApplyForce(force, m_PhysicsObject->GetWorldCenter(), true);
}

void GameLogic::rotate(float angle) {
	m_PhysicsObject->SetAngularVelocity(angle);
}

*/
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


void GameLogic::handleStates() {
	switch (game_state) {
	case eGameState::GS_FIRING:
		if (!isAwake("bomb", "")) {
			out_audio_events.push_back(eAudioEvents::AE_EXPLOSION_BOMB);
			game_state = eGameState::GS_EXPLODING;
		}
		break;

	case eGameState::GS_EXPLODING:
		Explosion* e = static_cast<Explosion*>(gm->getEntityByName("bomb_explosion", ""));
		if (e->current_life > 0) {
			PhysicsObject* bomb = gm->getEntityByName("bomb", "")->getPhysicsObject();
			gm->getEntityByName("bomb_explosion", "")->getPhysicsObject()->body->SetTransform(b2Vec2(bomb->getPos().x, bomb->getPos().y), 0.0f);
			gm->getEntityByName("bomb_explosion", "")->getPhysicsObject()->body->SetActive(true);
			gm->getEntityByName("bomb_explosion", "")->is_renderable = true;
			e->current_life--;
		}
		else {
			e->current_life = e->lifetime;
			gm->getEntityByName("bomb_explosion", "")->getPhysicsObject()->body->SetActive(false);
			gm->getEntityByName("bomb_explosion", "")->is_renderable = false;
			game_state = GS_PLAYING;
		}
		break;
	}
}






bool GameLogic::isAwake(string name, string parent) {
	return gm->getEntityByName("bomb", "")->getPhysicsObject()->body->IsAwake();
}


// return the 3d mouse position.
Vector3 GameLogic::getMousePos3D() {
	Vector2 pos = gm->getWindow()->GetOSMousePosition();
	Vector2 mPos = gm->getWindow()->convertToScreenCoords(pos);
	return Vector3(mPos.x, mPos.y, 5.0f);
}
