#include "GameLogic.h"

#define vec0 Vector3(0.0,0.0,0.0)
#define VEC_STILL Vector3(0.0,0.0,5.0)
#define CAM_OFFSET 20.0f
#define DAMPING 0.0003f
#define ROTATION 0.003f
#define MAX_CHARGE 20.0f
#define MAX 20
#define pointer_offset 5.0f
#define clamp 0.035f
#define charge_speed 0.02f
#define initial_charge 0.01f
#define hub_health 3
#define starting_resource 3;
#define p1_start_loc Vector3(-25, 0, -7.5)
#define p2_start_loc Vector3(25, 0, -7.5)

b2Vec2 GameLogic::force;

GameLogic::GameLogic(GameManager* gm, GameLogicManager* glm, b2World* world, AudioManager* am, Camera* cam){
	this->glm = glm;
	this->gm = gm;
	this->world = world;
	this->am = am;
	this->cam = cam;
}

GameLogic::~GameLogic(){
	
}
// logic events that occur at the start of the game
void GameLogic::init() {
	player_turn = ePlayerTurn::GS_PLAYER_1;
	game_state = eGameState::GS_PLAYING;
	p1 = new Player("p1", "", "", "", vec0, gm->iom->findMesh("mesh_player"), gm->iom->findShader("basic"), gm->iom->findTexture("rust"), false, false,
		false, true, 30, 1.0, world, 0.5, 1.0);
	p2 = new Player("p2", "", "", "", vec0, gm->iom->findMesh("mesh_player"), gm->iom->findShader("basic"), gm->iom->findTexture("rust"), false, false,
		false, true, 30, 2.0, world, 0.5, 1.0);

	p1->nodes.push_back( new NodeHub("p1_node_0", "", "", "", p1_start_loc,
		gm->iom->findMesh("mesh_hub"), gm->iom->findShader("basic"), gm->iom->findTexture("rust"), true, true,
		true, true, 30, 2.0, world, 0.5, 1.0, hub_health, active_player));
	p2->nodes.push_back( new NodeHub("p2_node_0", "", "", "", p2_start_loc,
		gm->iom->findMesh("mesh_hub"), gm->iom->findShader("basic"), gm->iom->findTexture("rust"), true, true,
		true, true, 30, 2.0, world, 0.5, 1.0, hub_health, active_player));
	
	gm->addEntity(p1->nodes[0]);
	gm->addEntity(p2->nodes[0]);

	p1->selected_node = p1->nodes[0];
	p2->selected_node = p2->nodes[0];
	active_player = p1;

	pointer = gm->getEntityByName("pointer", "");

	gm->getEntityByName("bomb", "")->getPhysicsObject()->body->SetLinearDamping(DAMPING);
	gm->getEntityByName("bomb", "")->getPhysicsObject()->body->SetActive(false);
	//gm->getEntityByName("bomb_explosion", "")->getPhysicsObject()->body->SetActive(false);
	
	setPointer();

	pointer->is_renderable = true;
	

	pointer->getPhysicsObject()->body->GetFixtureList()->SetFilterData(getFixture(eFilterSolid, eNoCollide));
}


b2Vec2 GameLogic::getTrajectory(Entity* origin) {
	float cosx = cos(origin->getPhysicsObject()->body->GetAngle());
	float siny = sin(origin->getPhysicsObject()->body->GetAngle());

	float charge = (elapsed_seconds.count() * charge_speed) + initial_charge;

	if (charge > clamp) {
		charge = clamp;
	}

	cosx = cosx * charge;
	siny = siny * charge;

	force = b2Vec2(cosx, siny);

	return force;
}

string GameLogic::getName() {
	std::stringstream ss;
	ss << active_player->name << "_node_" << active_player->nodes.size();
	return ss.str();
}

void GameLogic::launch() {
	Entity* hub;
	
	switch (action) {
	case GameLogic::AS_HUB:
		hub = new NodeHub(getName(), "", "", "", active_player->selected_node->getPhysicsObject()->getPos(),
			gm->iom->findMesh("mesh_hub"), gm->iom->findShader("basic"), gm->iom->findTexture("rust"), true, true, 
			true, true, 30, 2.0, world, 0.5, 1.0, hub_health, active_player);
		hub->getPhysicsObject()->body->SetLinearDamping(DAMPING);
		gm->addEntity(hub);
		launchNode(hub);
		fired_entity = hub;
		break;

	case GameLogic::AS_BOMB:

		fireWeapon(gm->getEntityByName("bomb", ""));
		fired_entity = gm->getEntityByName("bomb", "");
		break;
	}
	game_state = GS_FIRING;
}

void GameLogic::launchNode(Entity* e) {
	e->is_renderable = true;
	active_player->selected_node->getPhysicsObject()->body->GetFixtureList()->SetFilterData(createFilter(eFilterNonSolid, eNoCollide));

	gm->getEntityByName("bomb", "")->getPhysicsObject()->body->SetTransform(active_player->selected_node->getPhysicsObject()->body->GetPosition(), 0.0f);
	e->getPhysicsObject()->body->SetActive(true);
	e->getPhysicsObject()->body->GetFixtureList()->SetFilterData(getFixture(eFilterNonSolid, eCollide));

	e->getPhysicsObject()->body->SetLinearVelocity(getTrajectory(active_player->selected_node));
}

// postion the bomb entity and fire it with a given charge in the directon teh active player is facing
void GameLogic::fireWeapon(Entity* bomb) {
	
	bomb->is_renderable = true;
	active_player->selected_node->getPhysicsObject()->body->GetFixtureList()->SetFilterData(createFilter(eFilterNonSolid, eNoCollide));

	gm->getEntityByName("bomb", "")->getPhysicsObject()->body->SetTransform(active_player->selected_node->getPhysicsObject()->body->GetPosition(), 0.0f);
	bomb->getPhysicsObject()->body->SetActive(true);
	bomb->getPhysicsObject()->body->GetFixtureList()->SetFilterData(getFixture(eFilterNonSolid, eCollide));

	bomb->getPhysicsObject()->body->SetLinearVelocity(getTrajectory(active_player->selected_node));
	
}

// TODO: condense this
// positions the pointer relative to point and angle of player
void GameLogic::setPointer() {
	float p_angle = active_player->selected_node->getPhysicsObject()->body->GetAngle();
	float posx = active_player->selected_node->getPhysicsObject()->getPos().x;
	float posy = active_player->selected_node->getPhysicsObject()->getPos().y;
	float x = posx + (pointer_offset * cos(p_angle));
	float y = posy + (pointer_offset * sin(p_angle));
	pointer->getPhysicsObject()->body->SetTransform(b2Vec2(x, y), pointer->getPhysicsObject()->start_pos.z);

}

void GameLogic::update(float msec) {
	
	handleEvents(); // events first
	handleStates(); // then states

 }

// returns a fixture matching the given params. Box2d fixtures determine what collisions will register with this object
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

// states occur over multiple frames, this determines action that occur because of this
void GameLogic::handleStates() {

	// declare vars outside switch
	Explosion* e;

	switch (game_state) {
	case eGameState::GS_FIRING:
		if (!isAwake(fired_entity)) {
			
			
			game_state = eGameState::GS_CONTACT;
		}
		break;

	case eGameState::GS_CONTACT:

		switch (action) {
		case GameLogic::AS_HUB:
			out_audio_events.push_back(eAudioEvents::AE_POWERUP);
			game_state = eGameState::GS_BUILDING;
			break;
		case GameLogic::AS_BOMB:
			out_audio_events.push_back(eAudioEvents::AE_EXPLOSION_BOMB);
			fired_entity->is_renderable = false;

			explosions.push_back( new Explosion("bomb_ex", "", "", "", fired_entity->getPhysicsObject()->getPos(),
				gm->iom->findMesh("mesh_explosion_3"), gm->iom->findShader("basic"), gm->iom->findTexture("rust"), true, true,
				false, true, 0, 3.0, world, 0.5, 1.0, 80, 2));
			gm->addEntity(explosions[explosions.size()-1]);
			gm->removeEntity(fired_entity);
			game_state = eGameState::GS_EXPLODING;
			break;
		}

		break;

	case eGameState::GS_BUILDING:

		game_state = eGameState::GS_PLAYING;
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
		else if (e->current_life == 0) {
			e->current_life = e->lifetime;
			gm->getEntityByName("bomb_explosion", "")->getPhysicsObject()->body->SetActive(false);
			gm->getEntityByName("bomb_explosion", "")->is_renderable = false;
			active_player->selected_node->getPhysicsObject()->body->GetFixtureList()->SetFilterData(createFilter(eFilterSolid, eCollide));

			game_state = eGameState::GS_PLAYING;
		}
		break;

	case eGameState::GS_CHARGING:
		charge = charge++;
		if (!charging) {
			/*
			if (charge >= MAX_CHARGE) {
				charge = MAX_CHARGE;
			}
			*/
			end = std::chrono::system_clock::now();
			elapsed_seconds = end - start;
			launch();
			//game_state = eGameState::GS_FIRING;
		}
		break;
	}
}

// determine actions based on logic events that have been stacked between frame 
void GameLogic::handleEvents() {
	
	// reset per frame variables here
	charging = false;

	// handle input events
	for (int i = 0; i < in_input_events.size(); i++) {
		
 		switch (in_input_events[i]) {
		case eInputEvents::IE_LEFT_CLICK:
			if (game_state == eGameState::GS_PLAYING) {
				//active_player->getPhysicsObject()->body->ApplyForce(b2Vec2(100, 0.0), active_player->getPhysicsObject()->body->GetWorldCenter(), true);
				
				float cosx = cos(active_player->selected_node->getPhysicsObject()->body->GetAngle());
				float siny = sin(active_player->selected_node->getPhysicsObject()->body->GetAngle());
				cosx = cosx * 0.7;
				siny = siny * 0.7;
				b2Vec2 force = b2Vec2(cosx, siny);

				active_player->selected_node->getPhysicsObject()->body->ApplyLinearImpulse(force, active_player->selected_node->getPhysicsObject()->body->GetWorldCenter(), true);
			
				out_audio_events.push_back(eAudioEvents::AE_MOVE);
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
				//charge = 1;
				start = std::chrono::system_clock::now();
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
		
		case eInputEvents::IE_PAD1:
			if (game_state == eGameState::GS_PLAYING) {
				action = eActionSelection::AS_HUB;
			}
			break;
	
		case eInputEvents::IE_PAD4:
			if (game_state == eGameState::GS_PLAYING) {
				action = eActionSelection::AS_BOMB;
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

// adjust the direction of the active player. Used to determien which wya to fire or move
void GameLogic::adjustDirection(eInputEvents dir) {
	switch (dir) {
	case GameLogic::IE_LEFT:
		active_player->selected_node->getPhysicsObject()->body->SetAngularVelocity(-ROTATION);
		break;
	case GameLogic::IE_RIGHT:
		active_player->selected_node->getPhysicsObject()->body->SetAngularVelocity(ROTATION);
		break;
	}
}

// box 2d filter determine what collision will register with this object
b2Filter GameLogic::createFilter(eFilter filter, eMask mask) {
	b2Filter f;
	f.categoryBits = filter;
	f.maskBits = mask;
	return f;
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


// events that occur on turn swap after pressing enter. Changes active player and cam position
void GameLogic::endTurn() {
	
	if (player_turn == ePlayerTurn::GS_PLAYER_1) {
		player_turn = ePlayerTurn::GS_PLAYER_2;
		active_player = p2;
		Vector3 player_pos = p2->selected_node->getPhysicsObject()->getPos();
		cam->SetPosition(Vector3(player_pos.x, player_pos.y, CAM_OFFSET));
	}
	else {
		player_turn = ePlayerTurn::GS_PLAYER_1;
		active_player = p1;
		Vector3 player_pos = p1->selected_node->getPhysicsObject()->getPos();
		cam->SetPosition(Vector3(player_pos.x, player_pos.y, CAM_OFFSET));
	}
}

// check if an entity with given name is awake or not
bool GameLogic::isAwake(Entity* e) {
	return e->getPhysicsObject()->body->IsAwake();
}

// return the 3d mouse position.
Vector3 GameLogic::getMousePos3D() {
	Vector2 pos = gm->getWindow()->GetOSMousePosition();
	Vector2 mPos = gm->getWindow()->convertToScreenCoords(pos);
	return Vector3(mPos.x, mPos.y, 5.0f);
}
