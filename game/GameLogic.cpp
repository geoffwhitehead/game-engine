#include "GameLogic.h"
#include "Explosion.h"
#include <math.h>

#define VEC_STILL Vector3(0.0,0.0,5.0)
#define CAM_OFFSET 20.0f
#define DAMPING 0.0005f
#define ROTATION 0.001f
#define MAX_CHARGE 50.0f

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




	p1 = gm->getEntityByName("player_1", "");
	p2 = gm->getEntityByName("player_2", "");
	pointer = gm->getEntityByName("pointer", "");

	p1->getPhysicsObject()->body->SetLinearDamping(DAMPING);
	p2->getPhysicsObject()->body->SetLinearDamping(DAMPING);
	gm->getEntityByName("bomb", "")->getPhysicsObject()->body->SetLinearDamping(DAMPING);
	gm->getEntityByName("bomb", "")->getPhysicsObject()->body->SetActive(false);
	gm->getEntityByName("bomb_explosion", "")->getPhysicsObject()->body->SetActive(false);
	

	setPointer();
	pointer->is_renderable = true;
	
	pointer->getPhysicsObject()->body->GetFixtureList()->SetFilterData(getFixture(eFilterSolid, eNoCollide));

	


}

// TODO: condense this
// positions the pointer relative to point and angle of player
void GameLogic::setPointer() {
	float p1angle = p1->getPhysicsObject()->body->GetAngle();
	float posx = p1->getPhysicsObject()->getPos().x;
	float posy = p1->getPhysicsObject()->getPos().y;
	float x = posx + (25 * cos(p1angle));
	float y = posy + (25 * cos(p1angle));
	pointer->getPhysicsObject()->body->SetTransform(b2Vec2(x, y), pointer->getPhysicsObject()->start_pos.z);

}

void GameLogic::update(float msec) {
	
	handleEvents(); // events first
	handleStates(); // then states

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

void GameLogic::handleStates() {

	// declare vars outside switch
	Explosion* e;

	switch (game_state) {
	case eGameState::GS_FIRING:
		if (!isAwake("bomb", "")) {
			out_audio_events.push_back(eAudioEvents::AE_EXPLOSION_BOMB);
			gm->getEntityByName("bomb", "")->is_renderable = false;
			game_state = eGameState::GS_EXPLODING;
		}
		break;

	case eGameState::GS_EXPLODING:
		e = static_cast<Explosion*>(gm->getEntityByName("bomb_explosion", ""));
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
			gm->getEntityByName("player_1", "")->getPhysicsObject()->body->GetFixtureList()->SetFilterData(createFilter(eFilterSolid, eCollide));

			game_state = eGameState::GS_PLAYING;
		}
		break;

	case eGameState::GS_CHARGING:
		charge++;
		if (!charging) {
			if (charge >= MAX_CHARGE) {
				charge = MAX_CHARGE;
			}
			fireWeapon(charge);
			game_state = eGameState::GS_FIRING;
		}

		break;
	}

}

// determine actions based on logic events that have been stacked between frame 
void GameLogic::handleEvents() {
	// handle input events
	
	
	// reset per frame variables here
	charging = false;

	//

	for (int i = 0; i < in_input_events.size(); i++) {
		
 		switch (in_input_events[i]) {
			Entity* e;
		case eInputEvents::IE_LEFT_CLICK:
			if (game_state == eGameState::GS_PLAYING) {
				cout << "left click" << endl;
				e = gm->getEntityByName("player_1", "");
				e->getPhysicsObject()->body->ApplyForce(b2Vec2(0.1, 0.01), e->getPhysicsObject()->body->GetWorldCenter(), true);
				out_audio_events.push_back(eAudioEvents::AE_TURN_SWAP);
			}
			break;
		case eInputEvents::IE_ENTER:
			if (game_state == eGameState::GS_PLAYING) {
				out_audio_events.push_back(eAudioEvents::AE_TURN_SWAP);
				endTurn();
			}
			break;
		case eInputEvents::IE_SPACE:
			if (game_state == eGameState::GS_PLAYING) {
				game_state = eGameState::GS_CHARGING;
				charge = 0;
			}
			if (game_state == eGameState::GS_CHARGING) {
				charging = true;
			}
			break;
		case eInputEvents::IE_LEFT:
			if (game_state == eGameState::GS_PLAYING) {
				adjustDirection(eInputEvents::IE_LEFT);
				setPointer();
			}
			break;
		case eInputEvents::IE_RIGHT:
			if (game_state == eGameState::GS_PLAYING) {
				adjustDirection(eInputEvents::IE_RIGHT);
				setPointer();
			}
			break;
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

void GameLogic::adjustDirection(eInputEvents dir) {
	switch (dir) {
	case GameLogic::IE_LEFT:
		p1->getPhysicsObject()->body->SetAngularVelocity(-ROTATION);
		break;
	case GameLogic::IE_RIGHT:
		p1->getPhysicsObject()->body->SetAngularVelocity(ROTATION);
		break;
	}
}

b2Filter GameLogic::createFilter(eFilter filter, eMask mask) {
	b2Filter f;
	f.categoryBits = filter;
	f.maskBits = mask;
	return f;
}

void GameLogic::fireWeapon(float charge) {
	Entity* bomb = gm->getEntityByName("bomb", "");
	bomb->is_renderable = true;

	gm->getEntityByName("player_1", "")->getPhysicsObject()->body->GetFixtureList()->SetFilterData(createFilter(eFilterNonSolid, eNoCollide));

	gm->getEntityByName("bomb", "")->getPhysicsObject()->body->SetTransform(p1->getPhysicsObject()->body->GetPosition(), 0.0f);
	bomb->getPhysicsObject()->body->SetActive(true);
	bomb->getPhysicsObject()->body->GetFixtureList()->SetFilterData(getFixture(eFilterSolid, eCollide));
	
	
	b2Vec2 force = b2Vec2((cos(p1->getPhysicsObject()->body->GetAngle())) * charge, (sin(p1->getPhysicsObject()->body->GetAngle())) * charge);
	
	bomb->getPhysicsObject()->body->ApplyLinearImpulse(force, p1->getPhysicsObject()->body->GetWorldCenter(), true);
	
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








bool GameLogic::isAwake(string name, string parent) {
	return gm->getEntityByName("bomb", "")->getPhysicsObject()->body->IsAwake();
}


// return the 3d mouse position.
Vector3 GameLogic::getMousePos3D() {
	Vector2 pos = gm->getWindow()->GetOSMousePosition();
	Vector2 mPos = gm->getWindow()->convertToScreenCoords(pos);
	return Vector3(mPos.x, mPos.y, 5.0f);
}
