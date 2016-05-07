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
#define starting_resource 4;
#define explosion_group "explosions"
#define hub_group "hubs"
#define bomb_group "bombs"
#define hub_radius 2.0
#define bomb_radius 1.0
#define node_damage 2
#define kill 100
#define board_depth -7.5
#define p1_mesh "p1_mesh_hub"
#define p2_mesh "p2_mesh_hub"

const int hub_cost = 2;
const int bomb_cost = 1;

const Vector3 p1_start_loc = Vector3(-25, 0, -7.5);
const Vector3 p2_start_loc = Vector3(25, 0, -7.5);

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
		false, true, 30, 1.0, world, 0.5, 1.0, p1_mesh);
	p2 = new Player("p2", "", "", "", vec0, gm->iom->findMesh("mesh_player"), gm->iom->findShader("basic"), gm->iom->findTexture("rust"), false, false,
		false, true, 30, 2.0, world, 0.5, 1.0, p2_mesh);

	p1->nodes.push_back( new NodeHub("p1_node_0", "", hub_group, "", p1_start_loc,
		gm->iom->findMesh(p1_mesh), gm->iom->findShader("basic"), gm->iom->findTexture("rust"), true, true,
		true, true, 30, hub_radius, world, 0.5, 1.0, hub_health, p1, hub_cost));
	p2->nodes.push_back( new NodeHub("p2_node_0", "", hub_group, "", p2_start_loc,
		gm->iom->findMesh(p2_mesh), gm->iom->findShader("basic"), gm->iom->findTexture("rust"), true, true,
		true, true, 30, hub_radius, world, 0.5, 1.0, hub_health, p2, hub_cost));
	
	gm->addEntity(p1->nodes[0]);
	gm->addEntity(p2->nodes[0]);

	p1->selected_node = p1->nodes[0];
	p2->selected_node = p2->nodes[0];
	
	// set the starting resource
	p1->total_resource = starting_resource;
	p1->current_resource = starting_resource;
	p2->total_resource = starting_resource;
	p2->current_resource = starting_resource;

	active_player = p1;
	cam->SetPosition(Vector3(p1_start_loc.x, p1_start_loc.y, CAM_OFFSET));

	pointer = gm->getEntityByName("pointer", "");
	setPointer();
	pointer->is_renderable = true;
	setFixture(pointer, eFilterSolid, eNoCollide);
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
	LevelEntity* hub;
	LevelEntity* b1;
	
	switch (action) {
	case GameLogic::AS_HUB:

		hub = new NodeHub(getName(), "", hub_group, "", active_player->selected_node->getPhysicsObject()->getPos(),
			gm->iom->findMesh(active_player->player_mesh), gm->iom->findShader("basic"), gm->iom->findTexture("rust"), true, true, 
			true, true, 30, hub_radius, world, 0.5, 1.0, hub_health, active_player, hub_cost);
		hub->getPhysicsObject()->body->SetLinearDamping(DAMPING);
		// after creating add new hub to all the game vectors
		gm->addEntity(hub);
		active_player->nodes.push_back(hub);
		fired_entity = hub;
		launchNode(fired_entity);
		break;

	case GameLogic::AS_BOMB:
		// create a new bomb
		b1 = new Bomb("bomb_1", "", bomb_group, "", active_player->selected_node->getPhysicsObject()->getPos(),
			gm->iom->findMesh("mesh_bomb"), gm->iom->findShader("basic"), gm->iom->findTexture("rust"), true, true,
			true, true, 0, bomb_radius, world, 0.5, 1.0, bomb_cost);

		b1->getPhysicsObject()->body->SetLinearDamping(DAMPING);
		gm->addEntity(b1);

		fireWeapon(b1);
		fired_entity = b1;
		break;
	}
	game_state = GS_FIRING;
}

void GameLogic::launchNode(Entity* e) {
	setFixture(e, eFilterNonSolid, eNoCollide);
	e->getPhysicsObject()->body->SetLinearVelocity(getTrajectory(active_player->selected_node));
}

// postion the bomb entity and fire it with a given charge in the directon teh active player is facing
void GameLogic::fireWeapon(Entity* bomb) {
	
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

void GameLogic::handleCollisions() {
	for (int i = 0; i < in_contact_events.size(); i++){
		if (in_contact_events[i].first->group == explosion_group){
			if (in_contact_events[i].second->group == hub_group) {
				applyDamage(static_cast<Node*>(in_contact_events[i].second), static_cast<Explosion*>(in_contact_events[i].first)->damage);
			}
		}
		else if (in_contact_events[i].first->group == hub_group){
			if (in_contact_events[i].second->group == explosion_group) {
				applyDamage(static_cast<Node*>(in_contact_events[i].first), static_cast<Explosion*>(in_contact_events[i].second)->damage);
			}
			if (in_contact_events[i].second->group == hub_group) {
				
				if (static_cast<Node*>(in_contact_events[i].first)->created_on > static_cast<Node*>(in_contact_events[i].second)->created_on) {
					applyDamage(static_cast<Node*>(in_contact_events[i].first), kill);
					applyDamage(static_cast<Node*>(in_contact_events[i].second), node_damage);
				}
				else {
					applyDamage(static_cast<Node*>(in_contact_events[i].first), node_damage);
					applyDamage(static_cast<Node*>(in_contact_events[i].second), kill);
				}
			}
		}
	}
	// clear
	in_contact_events.clear();

}

void GameLogic::applyDamage(Node* n, int damage) {
	if (n != active_player->selected_node) {
		out_audio_events.push_back(eAudioEvents::AE_HUB_DAMAGED);
		cout << "damage" << endl;
		n->health = n->health - damage;

		if (n->health <= 0){
			destroyNode(n);
			out_audio_events.push_back(eAudioEvents::AE_HUB_DESTROYED);
		}
	}
}

void GameLogic::destroyNode(Node* n) {
	//delete reference in nodes
	n->owner->nodes.erase(std::remove(n->owner->nodes.begin(), n->owner->nodes.end(), n), n->owner->nodes.end());
	// then schedule the deletion with the gm
	gm->markToDelete(n);
	// create a game event to say a node has been destroyed. The event will check whether there are remaining nodes
	in_game_events.push_back(eGameEvents::GE_NODE_DESTROYED);
	
}


void GameLogic::update(float msec) {

	handleCollisions(); // collisions
	handleEvents(); // events first
	world->SetAllowSleeping(true); // in states the bit mask of a hub is changed after sleeping. Need to wake it up to register any collisions and then reset it here, before the next step.
	handleStates(); // then states

 }


void GameLogic::setFixture(Entity* e, enum eFilter f, enum eMask m) {
	
	//get the existing filter
	b2Filter filter = e->getPhysicsObject()->body->GetFixtureList()->GetFilterData();
	filter.maskBits = m;
	filter.categoryBits = f;
	e->getPhysicsObject()->body->GetFixtureList()->SetFilterData(filter);
	
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
	bool live = false; // used for explosion -- determine whether any remaining
	Explosion*e;
	b2Vec2 contact_pos;

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

			e = new Explosion("bomb_ex", "", explosion_group, "", fired_entity->getPhysicsObject()->getPos(),
				gm->iom->findMesh("mesh_explosion_3"), gm->iom->findShader("basic"), gm->iom->findTexture("rust"), true, true,
				true, true, 0, 3.0, world, 0.5, 1.0, 80, 2);

			explosions.push_back( e );

			gm->addEntity(e);
			gm->markToDelete(fired_entity);
			fired_entity = 0;
			game_state = eGameState::GS_EXPLODING;
			break;
		}

		break;

	case eGameState::GS_BUILDING:
		// active the hub so collisions occur

		world->SetAllowSleeping(false); // this is important so that collision of a stationary object is registered when placed inside another statuonary object
		contact_pos = b2Vec2(fired_entity->getPhysicsObject()->getPos().x, fired_entity->getPhysicsObject()->getPos().y);
		setFixture(fired_entity, eFilterNonSolid, eCollide);

		fired_entity = 0;
		game_state = eGameState::GS_PLAYING;
		break;

	case eGameState::GS_EXPLODING:
		
		for (int i = 0; i < explosions.size(); i++)
		{
			if (explosions[i]->current_life > 0) {
				explosions[i]->current_life--;
				live = true;
			}
			else if (explosions[i]->current_life == 0) {
				gm->markToDelete(explosions[i]);
				explosions.erase(std::remove(explosions.begin(), explosions.end(), explosions[i]), explosions.end()); // clear from vector -- leave here
			}
			break;
		}
		if (live == false){
			game_state = eGameState::GS_PLAYING;
		}
		break;

	case eGameState::GS_CHARGING:
		charge = charge++;
		if (!charging) {
			end = std::chrono::system_clock::now();
			elapsed_seconds = end - start;
			launch();
		}
		break;

	case eGameState::GS_CAMERA_MOVING:
		// set the new position
		Vector3 c_pos = cam->GetPosition();
		float new_x = cam->GetPosition().x + direction_step.x;
		float new_y = cam->GetPosition().y + direction_step.y;
		cam->SetPosition(Vector3(new_x, new_y, c_pos.z));

		// reduce the steps
		step_counter--;

		// check if move finished and change gamestate if true
		if (step_counter == 0) {
			game_state = eGameState::GS_PLAYING;
		}


		break;

	}

}

LevelEntity* GameLogic::findNextNode() {
	// find the index of selected node
	for (int i = 0; i < active_player->nodes.size(); i++) {
		if (active_player->nodes[i] == active_player->selected_node) {
			return active_player->nodes[(i+1) % (active_player->nodes.size())];
		}
	}
}

bool GameLogic::sufficientResource() {
	int cost;
	switch (action) {
	case eActionSelection::AS_HUB:
		cost = hub_cost;
		break;
	case eActionSelection::AS_BOMB:
		cost = bomb_cost;
		break;
	}

	if (cost <= active_player->current_resource) {
		active_player->current_resource = active_player->current_resource - cost;
		cout << "2: " << active_player->current_resource << " " << cost << endl;
		return true;
	}
	return false;
}

// determine actions based on logic events that have been stacked between frame 
void GameLogic::handleEvents() {
	
	// reset per frame variables here
	charging = false;


	// handle in game events
	for (int i = 0; i < in_game_events.size(); i++) {
		switch (in_game_events[i]) {
		case eGameEvents::GE_QUIT:
			gm->getWindow()->forceQuit = true;
			break;
		case eGameEvents::GE_NODE_DESTROYED:
			if (p1->nodes.size() == 0) {
				cout << "Player 2 WINS!!!!!!" << endl;
				game_state = eGameState::GS_GAME_END;
			}
			if (p2->nodes.size() == 0) {
				cout << "Player 2 WINS!!!!!!" << endl;
				game_state = eGameState::GS_GAME_END;
			}
			break;
		}
	}
	// game events exist in game logic so need to be cleared here... unlike other events
	in_game_events.clear();



	// handle input events
	for (int i = 0; i < in_input_events.size(); i++) {
		
 		switch (in_input_events[i]) {
		case eInputEvents::IE_LEFT_CLICK:
			if (game_state == eGameState::GS_PLAYING) {
				//active_player->getPhysicsObject()->body->ApplyForce(b2Vec2(100, 0.0), active_player->getPhysicsObject()->body->GetWorldCenter(), true);
				
				float cosx = cos(active_player->selected_node->getPhysicsObject()->body->GetAngle());
				float siny = sin(active_player->selected_node->getPhysicsObject()->body->GetAngle());
				cosx = cosx * 0.1;
				siny = siny * 0.1;
				b2Vec2 force = b2Vec2(cosx, siny);

				active_player->selected_node->getPhysicsObject()->body->ApplyLinearImpulse(force, active_player->selected_node->getPhysicsObject()->body->GetWorldCenter(), true);
			
				out_audio_events.push_back(eAudioEvents::AE_MOVE);
			}
			break;

		case eInputEvents::IE_KEY_TAB:
			if (game_state == eGameState::GS_PLAYING) {

				// only move the camera if player holds more than one node
				if (active_player->nodes.size() > 1) {
					// reset the camera step counter
					step_counter = camera_steps;
					// find the position of the next node and set it as the active node
					LevelEntity* next_node = findNextNode();
					active_player->selected_node = next_node;
					target_pos = Vector2(next_node->getPhysicsObject()->getPos().x, next_node->getPhysicsObject()->getPos().y);
					cout << target_pos << endl;
					// calculate the direction and then divide it by the number of steps (frames) to issue the cam moveennt over
					direction_step = target_pos - Vector2(cam->GetPosition().x, cam->GetPosition().y);
					direction_step = Vector2(direction_step.x / camera_steps, direction_step.y / camera_steps);
					// change the game state whilst hte camera is moving to lock out other actions
					game_state = eGameState::GS_CAMERA_MOVING;
				} else {
					active_player->selected_node == active_player->nodes[0];
				}
			}
			break;

		case eInputEvents::IE_ENTER:
			if (game_state == eGameState::GS_PLAYING) {
				out_audio_events.push_back(eAudioEvents::AE_TURN_SWAP);
				std::cout << "0: " << active_player->current_resource << " " << active_player->total_resource << endl;

				endTurn();
			}
			break;
		case eInputEvents::IE_SPACE:
			std::cout << "0: " << active_player->current_resource << " " << active_player->total_resource << endl;

			if (game_state == eGameState::GS_PLAYING) {
				std::cout << "1: " << active_player->current_resource << " " << active_player->total_resource << endl;

				if (sufficientResource()) {
					cout << "3: " << active_player->current_resource << " " << active_player->total_resource << endl;
					game_state = eGameState::GS_CHARGING;
					start = std::chrono::system_clock::now();
				}
				else {
					out_audio_events.push_back(eAudioEvents::AE_INSUF_RESOURCE);
				}
				
			}
			if (game_state == eGameState::GS_CHARGING) {
				charging = true;
			}
			break;
		case eInputEvents::IE_LEFT:
			std::cout << "0: " << active_player->current_resource << " " << active_player->total_resource << endl;

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
	}
	else {
		player_turn = ePlayerTurn::GS_PLAYER_1;
		active_player = p1;
	}
	// reset the resource
	active_player->current_resource = active_player->total_resource;
	// reset the step counter
	step_counter = camera_steps;
	// find the position of the next node and set it as the active node
	Entity* next_node = active_player->selected_node;
	target_pos = Vector2(next_node->getPhysicsObject()->getPos().x, next_node->getPhysicsObject()->getPos().y);
	// calculate the direction and then divide it by the number of steps (frames) to issue the cam moveennt over
	direction_step = target_pos - Vector2(cam->GetPosition().x, cam->GetPosition().y);
	direction_step = Vector2(direction_step.x / camera_steps, direction_step.y / camera_steps);
	// change the game state whilst hte camera is moving to lock out other actions
	game_state = eGameState::GS_CAMERA_MOVING;
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
