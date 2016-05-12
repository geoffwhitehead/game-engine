#include "GameLogic.h"


#define ppm 30
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

void GameLogic::destroy() {

}
// logic events that occur at the start of the game
void GameLogic::init() {
	// default starting player and set the correct state
	player_turn = ePlayerTurn::GS_PLAYER_1;
	game_state = eGameState::GS_PLAYING;

	//create the player entities
	p1 = new Player("p1", "", "", "", vec0, gm->iom->findMesh("mesh_player"), gm->iom->findShader("basic"), gm->iom->findTexture("rust"), false, false,
		false, true, ppm, 0.0, world, 0.5, 1.0, p1_mesh);
	p2 = new Player("p2", "", "", "", vec0, gm->iom->findMesh("mesh_player"), gm->iom->findShader("basic"), gm->iom->findTexture("rust"), false, false,
		false, true, ppm, 0.0, world, 0.5, 1.0, p2_mesh);

	// create the inital starting nodes for each player
	NodeHub* n1 = new NodeHub("p1_node_0", "", group_hub, subgroup_hub, constructVector(p1_start_loc, hub_z),
		gm->iom->findMesh(p1_mesh + node_hub), gm->iom->findShader("basic"), gm->iom->findTexture("rust"), true, true,
		true, true, ppm, hub_radius, world, 0.5, 1.0, hub_health, p1, hub_cost);
	NodeHub* n2 =  new NodeHub("p2_node_0", "", group_hub, subgroup_hub, constructVector(p2_start_loc, hub_z),
		gm->iom->findMesh(p2_mesh + node_hub), gm->iom->findShader("basic"), gm->iom->findTexture("rust"), true, true,
		true, true, ppm, hub_radius, world, 0.5, 1.0, hub_health, p2, hub_cost);
	
	// set the starting node fixtures
	setFixture(n1, eNode, eNode | eExplosion | eBlock | eConnector | eDisablers);
	setFixture(n2, eNode, eNode | eExplosion | eBlock | eConnector | eDisablers);

	// assign them to aplayer
	p1->nodes.push_back(n1);
	p2->nodes.push_back(n2);
	
	// notify the connection manager
	ConnectionManager::addNode(n1);
	ConnectionManager::addNode(n2);

	
	// add the player entities
	gm->addEntity(p1->nodes[0]);
	gm->addEntity(p2->nodes[0]);

	// set a default selected node
	p1->selected_node = p1->nodes[0];
	p2->selected_node = p2->nodes[0];
	
	// set the collision fixtures for the level entities
	for (int i = 0; i < gm->entities.size(); i++) {
		if (gm->entities[i]->group == group_env_block) {
			setFixture(gm->entities[i], eBlock, eNode );
		}
		if (gm->entities[i]->group == group_env_resource) {
			setFixture(gm->entities[i], eResource | eBlock, eNode );
		}
	}
	// set the starting resource
	p1->total_resource = starting_resource;
	p1->current_resource = starting_resource;
	p2->total_resource = starting_resource;
	p2->current_resource = starting_resource;

	// set the player to start first and position the cam accordingly
	active_player = p1;
	cam->SetPosition(constructVector(p1_start_loc, camera_z));

	// set the pointer entity properties
	pointer = gm->getEntityByName("pointer", "");
	pointer_mark = gm->getEntityByName("pointer_centre", "");
	setPointer();
	setFixture(pointer, ePointer, eNoCollide);
	setFixture(pointer_mark, ePointer, eNoCollide);

	showInstructions();

	cout << "Turn: Player 1" << endl;
	cout << "-> Resources: " << active_player->current_resource << "/" << active_player->total_resource << endl;
	cout << "-> Active Nodes: " << active_player->nodes.size() << endl;
	out_audio_events.push_back(eAudioEvents::AE_START);
}

void GameLogic::showInstructions() {

	out_audio_events.push_back(eAudioEvents::AE_SELECT);
	cout << endl;
	cout << "Info: " << endl;
	cout << "-> Aim: destroy your opponents nodes." << endl;
	cout << "-> Controls: Numpad 1-3   - Nodes" << endl;
	cout << "             Numpad 3-6   - Weapons" << endl;
	cout << "             Numpad +     - Repair" << endl;
	cout << "             Enter        - End Turn" << endl;
	cout << "             Space        - Charge Weapon (Release to fire)" << endl;
	cout << "             A            - Previous node" << endl;
	cout << "             D            - Select next node" << endl;
	cout << "             Left/Right   - Rotate firing angle" << endl;
	cout << "             TAB          - Show Instructions" << endl;
	cout << endl;
}

/*****************************************************
GAME UPDATE
******************************************************/

void GameLogic::update(float msec) {
	checkResourceHubs();
	handleCollisions(); // collisions
	handleEvents(); // events first
	handleStates(); // then states

}

void GameLogic::checkResourceHubs() {
	for (int i = 0; i < active_player->resource_nodes.size(); i++) {
		if (!active_player->resource_nodes[i]->is_powered_on) {
			!active_player->resource_nodes[i]->lifespan--;

			if (active_player->resource_nodes[i]->lifespan == 0) {
				applyDamage(active_player->resource_nodes[i], kill);
			}
			else if (active_player->resource_nodes[i]->lifespan == active_player->resource_nodes[i]->lifespan / 2) {

			}
		}
	}
}

b2Vec2 GameLogic::getTrajectory(Entity* origin) {
	
	float cosx;
	float siny;

	// special case for cluster wheere multiple projectiles are fired
	if (action == eActionSelection::AS_CLUSTER) {
		switch (cluster_index) {
		case 0:
			cosx = cos(origin->getPhysicsObject()->body->GetAngle()- cluster_spread);
			siny = sin(origin->getPhysicsObject()->body->GetAngle()- cluster_spread);
			break;
		case 1:
			cosx = cos(origin->getPhysicsObject()->body->GetAngle());
			siny = sin(origin->getPhysicsObject()->body->GetAngle());
			break;
		case 2:
			cosx = cos(origin->getPhysicsObject()->body->GetAngle()+ cluster_spread);
			siny = sin(origin->getPhysicsObject()->body->GetAngle()+ cluster_spread);
			break;
		}
	}
	else { // for everything else ... use this
		cosx = cos(origin->getPhysicsObject()->body->GetAngle());
		siny = sin(origin->getPhysicsObject()->body->GetAngle());
	}
	// calculate the power based on the elapsed seconds from holding and releasing space
	float charge = (elapsed_seconds.count() * charge_speed) + initial_charge;

	if (charge > clamp) {
		charge = clamp;
	}

	// calculate the trajectory and return
	cosx = cosx * charge;
	siny = siny * charge;
	force = b2Vec2(cosx, siny);

	return force;
}

string GameLogic::getName() {
	std::stringstream ss;
	switch (action) {
	case GameLogic::AS_HUB:
		ss << active_player->name << "_node_" << active_player->nodes.size();
		break;
	case GameLogic::AS_RESOURCE_HUB:
		ss << active_player->name << "_node_res_" << active_player->resource_nodes.size();
		break;
	case GameLogic::AS_SHIELD_HUB:
		ss << active_player->name << "_node_shield_" << active_player->resource_nodes.size();
		break;
	case GameLogic::AS_BOMB:
		ss << "bomb_1";
		break;
	}
	return ss.str();
}


/*****************************************************
LAUNCH
******************************************************/

void GameLogic::launch() {
	Node* hub;
	NodeHubResource* res_hub;
	NodeHubShield* shield_hub;
	LevelEntity* b1;


	out_audio_events.push_back(eAudioEvents::AE_LAUNCH);
	switch (action) {
	case GameLogic::AS_HUB:

		hub = new NodeHub(getName(), "", group_hub, "", constructVector(active_player->selected_node->getPhysicsObject()->getPos(), hub_z),
			gm->iom->findMesh(active_player->player_mesh + node_hub), gm->iom->findShader("basic"), gm->iom->findTexture("rust"), true, true, 
			true, true, ppm, hub_radius, world, 0.5, 1.0, hub_health, active_player, hub_cost);
		hub->getPhysicsObject()->body->SetLinearDamping(damping);
		// after creating add new hub to all the game vectors
		gm->addEntity(hub);
		ConnectionManager::addNode(hub);
		active_player->nodes.push_back(hub);
		fired_entity = hub;
		launchNode(fired_entity);
		game_state = GS_FIRING;
		break;

	case GameLogic::AS_RESOURCE_HUB:

		res_hub = new NodeHubResource(getName(), "", group_hub, subgroup_hub_resource, constructVector(active_player->selected_node->getPhysicsObject()->getPos(), hub_z),
			gm->iom->findMesh(active_player->player_mesh + node_resource_hub), gm->iom->findShader("basic"), gm->iom->findTexture("rust"), true, true,
			true, true, ppm, resource_hub_radius, world, 0.5, 1.0, resource_hub_health, active_player, resource_hub_cost, resource_hub_strength);
		res_hub->getPhysicsObject()->body->SetLinearDamping(damping);
		// after creating add new resource hub to all the game vectors
		gm->addEntity(res_hub);
		ConnectionManager::addNode(res_hub);
		active_player->resource_nodes.push_back(res_hub);
		fired_entity = res_hub;
		launchNode(fired_entity);
		game_state = GS_FIRING;
		break;

	case GameLogic::AS_SHIELD_HUB:

		shield_hub = new NodeHubShield(getName(), "", group_hub, subgroup_hub_shield, constructVector(active_player->selected_node->getPhysicsObject()->getPos(), hub_z),
			gm->iom->findMesh(active_player->player_mesh + node_shield_hub), gm->iom->findShader("basic"), gm->iom->findTexture("rust"), true, true,
			true, true, ppm, shield_hub_radius, world, 0.5, 1.0, shield_hub_health, active_player, shield_power);
		shield_hub->getPhysicsObject()->body->SetLinearDamping(damping);
		// after creating add new resource hub to all the game vectors
		gm->addEntity(shield_hub);
		ConnectionManager::addNode(shield_hub);
		active_player->shield_nodes.push_back(shield_hub);
		fired_entity = shield_hub;
		launchNode(fired_entity);
		game_state = GS_FIRING;
		break;

	case GameLogic::AS_BOMB:
		// create a new bomb
		b1 = new Bomb(getName(), "", group_bomb, "", constructVector(active_player->selected_node->getPhysicsObject()->getPos(), bomb_z),
			gm->iom->findMesh("mesh_bomb"), gm->iom->findShader("basic"), gm->iom->findTexture("rust"), true, true,
			true, true, 0, bomb_radius, world, 0.5, 1.0, bomb_cost);

		b1->getPhysicsObject()->body->SetLinearDamping(damping);
		setFixture(b1, eProjectile, eShield);
		gm->addEntity(b1);

		fireWeapon(b1);
		fired_entity = b1;
		shield_collision_flag = false;
		game_state = GS_FIRING;
		break;

	case GameLogic::AS_CLUSTER:
		game_state = GS_FIRING_CLUSTERS;

		shield_collision_flag = false;
		// reset the vars for cluster
		all_clusters_fired = false;
		cluster_counter = cluster_firing_time;
		cluster_index = 0;
		clusters_fired = 0;
		// initialise to nullptr. camera checks for null when following
		cluster_ptr_to_follow = nullptr;
		break;

	case GameLogic::AS_SURGE_BOMB:
		// create a new bomb
		b1 = new Bomb(getName(), "", group_surge_bomb, "", constructVector(active_player->selected_node->getPhysicsObject()->getPos(), bomb_z),
			gm->iom->findMesh(mesh_surge_bomb), gm->iom->findShader("basic"), gm->iom->findTexture("rust"), true, true,
			true, true, 0, surge_bomb_radius, world, 0.5, 1.0, surge_cost);

		b1->getPhysicsObject()->body->SetLinearDamping(damping);
		setFixture(b1, eProjectile, eShield );
		gm->addEntity(b1);

		fireWeapon(b1);
		fired_entity = b1;

		shield_collision_flag = false;
		surge_priority_found = false; // on contact the surge will only surge 1 entity. It will prioritise hubs over conenctors.
		game_state = GS_FIRING;
		break;

	case GameLogic::AS_REPAIR:

		b1 = new Bomb("repair", "", group_repair, "", constructVector(active_player->selected_node->getPhysicsObject()->getPos(), bomb_z),
			gm->iom->findMesh(mesh_repair), gm->iom->findShader("basic"), gm->iom->findTexture("rust"), true, true,
			true, true, 0, repair_radius, world, 0.5, 1.0, repair_cost);

		b1->getPhysicsObject()->body->SetLinearDamping(damping);
		setFixture(b1, eProjectile, eShield);
		gm->addEntity(b1);

		fireWeapon(b1);
		fired_entity = b1;
		shield_collision_flag = false;
		game_state = GS_FIRING;
		break;
	}
	
}

void GameLogic::launchNode(Entity* e) {
	setFixture(e, eNode, eShield);
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
	pointer->getPhysicsObject()->body->SetTransform(b2Vec2(x, y), pointer_z);
	pointer_mark->getPhysicsObject()->body->SetTransform(b2Vec2(active_player->selected_node->getPhysicsObject()->getPos().x, active_player->selected_node->getPhysicsObject()->getPos().y), 0);

}

void GameLogic::connectorIntersection(Connector* c1, Connector* c2) {
	// if both are 0 in GS_DESTROYING_CONNECTORS then finished
	destroy_left = 0;
	destroy_right = 0;
	Connector* new_con;
	// if connector 1 is newer
	if (c1->created_on > c2->created_on) {
		new_con = c1;
	}
	else { // else connector 2 is newer
		new_con = c2;
	}

	// check left and right and mark those to be destroyed if they exist
	if (new_con->left) {
		destroy_left = new_con->left;
	} else {
		// this connector is adjacent to a node on the left... remove the connection
		ConnectionManager::removeConnector(new_con->left_node, new_con);
	}
	if (new_con->right) {
		destroy_right = new_con->right;
	}
	else {
		// this connector is adjacent to a node on the right... remove the connection
		ConnectionManager::removeConnector(new_con->right_node, new_con);
	}

	// mark the initla connector to be destroyed
	destroyConnector(new_con);

	// this game state will iterate through all the connectors destroying them
	game_state = eGameState::GS_DESTROYING_CONNECTORS;
}

void GameLogic::surge(Connector*c) {
	// if both are 0 in GS_SURGE then finished
	surge_left = 0;
	surge_right = 0;
	left_node_surge = 0;
	right_node_surge = 0;
	left_surged = false;
	right_surged = false;
	current_surge_damage = surge_max_damage;

	// check left and right and mark those to surge to if they exist
	if (c->left) {
		surge_left = c->left;
	}
	else {
		left_node_surge = c->left_node;
	}
	if (c->right) {
		surge_right = c->right;
	}
	else {
		right_node_surge = c->right_node;
	}
	// this game state will iterate through all the connectors destroying them
	game_state = eGameState::GS_SURGE;

};

void GameLogic::applyRepair(Node* n, float repair_amt) {
	if (n->health == n->max_health) {
		cout << "Node health already max!" << endl;
	}
	else {
		n->health == n->health + repair_amount;
		cout << "-> +" << repair_amount << " repair to node, health: " << n->health << "/" << n->max_health << endl;
	}
}


/*****************************************************
HANDLE COLLISIONS
******************************************************/

void GameLogic::handleCollisions() {
	for (int i = 0; i < in_contact_events.size(); i++){
		//cout << "contact in handle collisions" << endl;
		// CONNECTOR

		if (in_contact_events[i].first->group == group_connector) {
			if (in_contact_events[i].second->group == group_connector && !collision_flag) {
				collision_flag = true;
				connectorIntersection(static_cast<Connector*>(in_contact_events[i].first), static_cast<Connector*>(in_contact_events[i].second));
			}
		}

		// BOMB

		else if (in_contact_events[i].first->group == group_bomb || in_contact_events[i].first->group == group_surge_bomb || in_contact_events[i].first->group == group_repair) {
			if (in_contact_events[i].second->group == group_shield) {
				float32 dist = (in_contact_events[i].second->getPhysicsObject()->getPos() - in_contact_events[i].first->getPhysicsObject()->getPos()).Length();
				//cout << "dist: " << dist << "    :     " << "shield: " << shield_radius << endl;
				if (dist > shield_radius) {
					shield_collision_flag = true;
					in_contact_events[i].first->getPhysicsObject()->body->SetLinearVelocity(b2Vec2(0, 0));
				} 
			}
		}
		

		//EXPLOSION
		else if (in_contact_events[i].first->group == group_explosion){
			// check if the explosion is a repair first 
			if (in_contact_events[i].first->sub_group == subgroup_repair) {
				if (in_contact_events[i].second->group == group_hub) {
					if (!repair_flag) {
						applyRepair(static_cast<Node*>(in_contact_events[i].second), static_cast<Explosion*>(in_contact_events[i].first)->damage);
						repair_flag = true;
					}
				}
			}
			else if (in_contact_events[i].second->group == group_hub) {
				surge_priority_found = true;
				applyDamage(static_cast<Node*>(in_contact_events[i].second), static_cast<Explosion*>(in_contact_events[i].first)->damage);
			}
			else if (in_contact_events[i].first->sub_group == subgroup_surge_initial && in_contact_events[i].second->group == group_connector && !surge_flag) {
				surge_flag = true;
				first_surge_contact = static_cast<Connector*>(in_contact_events[i].second);
			}
		}
		// SHIELD
		else if (in_contact_events[i].first->group == group_shield && !(static_cast<Shield*>(in_contact_events[i].first)->owner == active_player)) {
			
			if (in_contact_events[i].second->group == group_explosion) {
				applyDamage(static_cast<Shield*>(in_contact_events[i].first), static_cast<Explosion*>(in_contact_events[i].second)->damage);
			}
		}
		// HUB
		else if (in_contact_events[i].first->group == group_hub){
			// hub - connector
			if (in_contact_events[i].second->group == group_connector && !collision_flag) {
				collision_flag = true;
				if (static_cast<Node*>(in_contact_events[i].first)->created_on > static_cast<Node*>(in_contact_events[i].second)->created_on) {
					applyDamage(static_cast<Node*>(in_contact_events[i].first), kill);
				} else {
					// just pass this function both connectors as we know the connection needs to be removed based
					// on the above statement declaring the hub older.
					connectorIntersection(static_cast<Connector*>(in_contact_events[i].second), static_cast<Connector*>(in_contact_events[i].second));
				}
			}

			// hub - block
			else if (in_contact_events[i].second->group == group_env_block) {
				activation_collision_flag = true;
				applyDamage(static_cast<Node*>(in_contact_events[i].first), kill);
			}

			// hub - resource
			else if (in_contact_events[i].second->group == group_env_resource) {
				if (in_contact_events[i].first->sub_group == subgroup_hub_resource) {
					applyResource(static_cast<NodeHubResource*>(in_contact_events[i].first));
				}
				else {
					activation_collision_flag = true;
					applyDamage(static_cast<Node*>(in_contact_events[i].first), kill);
				}
			}

			// -- hub - hub
			if (in_contact_events[i].second->group == group_hub) {
				activation_collision_flag = true;
				if (static_cast<Node*>(in_contact_events[i].first)->created_on > static_cast<Node*>(in_contact_events[i].second)->created_on) {
					applyDamage(static_cast<Node*>(in_contact_events[i].first), kill);
					applyDamage(static_cast<Node*>(in_contact_events[i].second), node_damage);
				}
				else {
					applyDamage(static_cast<Node*>(in_contact_events[i].first), node_damage);
					applyDamage(static_cast<Node*>(in_contact_events[i].second), kill);
				}
			}
			// shield hub specific
			else if (in_contact_events[i].first->sub_group == subgroup_hub_shield) {
				// shield hub - explosion
				if (in_contact_events[i].second->group == group_explosion) {
					surge_priority_found = true;
					applyDamage(static_cast<Node*>(in_contact_events[i].first), static_cast<Explosion*>(in_contact_events[i].second)->damage);
				}
				// shield hub - resource
				else if (in_contact_events[i].second->group == group_env_resource) {
					activation_collision_flag = true;
					applyDamage(static_cast<Node*>(in_contact_events[i].first), kill);
				}
			}
			// -- hub specific
			else if (in_contact_events[i].first->sub_group == subgroup_hub) {
				// hub - explosion
				if (in_contact_events[i].second->group == group_explosion) {
					applyDamage(static_cast<Node*>(in_contact_events[i].first), static_cast<Explosion*>(in_contact_events[i].second)->damage);
					surge_priority_found = true;
				}
			}
			// -- resource hub specific
			else if (in_contact_events[i].first->sub_group == subgroup_hub_resource) {
				// resource hub - explosion
				if (in_contact_events[i].second->group == group_explosion) {
					applyDamage(static_cast<NodeHubResource*>(in_contact_events[i].first), static_cast<Explosion*>(in_contact_events[i].second)->damage);
					surge_priority_found = true;
				}
			}
		}
	}

	if (surge_flag && !surge_priority_found) {
		surge(first_surge_contact);
		surge_flag = false;
	}
	// clear
	in_contact_events.clear();

}

void GameLogic::applyResource(NodeHubResource* n){
	// because box2d requires object deletions to be sceduled at the end of the world step. The collision with the resource
	// block could come after the collision with entity which destroyed it. Need to check that it hasnt already been removed from the 
	// players resource vector ... ie. destroyed
	if (!n->is_powered_on && exists((vector<LevelEntity*>*)&(active_player->resource_nodes), (LevelEntity*)n)) {
		out_audio_events.push_back(eAudioEvents::AE_POWERUP_RESOURCE);	
		n->owner->total_resource = active_player->total_resource + n->resource_per_turn;
		n->is_powered_on = true;
		//reset hubs health on power up
		n->health = resource_hub_health;
		cout << "-> +" << n->resource_per_turn << " resource per turn!" << endl;
	}
}

void GameLogic::detachResource(NodeHubResource* n) {
	// just incase the event occurs on a node already powered down
	if (n->is_powered_on) {
		n->owner->total_resource = active_player->total_resource - n->resource_per_turn;
		n->is_powered_on = false;
	}
	cout << "-> -" << n->resource_per_turn << " resource per turn!" << endl;
}

void GameLogic::applyDamage(Shield* shield, float damage) {

	shield->current_power = shield->current_power - damage;
	if (shield->current_power <= 0) {
		out_audio_events.push_back(eAudioEvents::AE_SHIELD_OFF);
		shield->is_renderable = false;
		setFixture(shield, eShield, eNoCollide);
	}                                       
}

void GameLogic::applyDamage(Node* n, float damage) {
	
	if (n != active_player->selected_node) { // cannot damage node that you are currently firing from. Happens when you fire too close to yourself
		out_audio_events.push_back(eAudioEvents::AE_HUB_DAMAGED);
		n->health = n->health - damage;

		if (n->health <= 0){
			fired_entity = 0; // release the pointer;
			cout << "-> Node destroyed!" << endl;
			destroyNode(n);
		
		} else {
			if (n->owner != active_player) {
				cout << "-> " << damage << " damage caused, Node health: " << n->health << endl;
			}
			else {
				cout << "-> " << damage << " damage taken, Node health: " << n->health << endl;
			}
		}
		
	}

}

bool GameLogic::exists(vector<LevelEntity*>* vector, LevelEntity* to_find) {
	if (std::find(vector->begin(), vector->end(), to_find) != vector->end()) {
		return true;
	}
	return false;
}

void GameLogic::getConnectorsToRemove(vector<Entity*>* results, Connector*c, enum eConDirection dir) {
	
	
	if (dir == CD_LEFT) {
		if (c->left) {
			getConnectorsToRemove(results, c->left, dir);
		}
		else {
			// remove the reference to edge from node
			ConnectionManager::removeConnector(c->left_node, c);
			// check whether the left node is still connected
			if (!ConnectionManager::isNodeConnected(c->left_node)) {
				// check to make sure this isnt the last node
				if (c->left_node->owner->nodes.size() > 1) {
					(*results).push_back(c->left_node);
					//destroyNode(c->left_node);
				}
			}
		}
	}
	else if (dir == CD_RIGHT) {
		if (c->right) {
			getConnectorsToRemove(results, c->right, dir);
		}
		else {
			// remove the reference to edge from node
			ConnectionManager::removeConnector(c->right_node, c);
			// check whether the left node is still connected
			if (!ConnectionManager::isNodeConnected(c->right_node)) {
				// check to make sure this isnt the last node
				if (c->right_node->owner->nodes.size() > 1) {
					(*results).push_back(c->right_node);
					//destroyNode(c->right_node);
				}
			}
		}
	}
	(*results).push_back(c);
}




void GameLogic::destroyConnector(Connector*c) {
	// stop any further collisions... who knows
	setFixture(c, eConnector, eNoCollide);
	// mark the entityt itself for deletion
	gm->markToDelete(c);
	// play sound
	out_audio_events.push_back(eAudioEvents::AE_CONNECTOR_DOWN);
}

void GameLogic::removeEdges(Node*n) {
	vector<Connector*>* cons = ConnectionManager::getEdges(n);
	vector<Entity*> edge_to_remove;
	for (int i = 0; i < cons->size(); i++) {
		if ((*cons)[i]->left_node == n) {
			getConnectorsToRemove(&edge_to_remove, (*cons)[i], CD_RIGHT);
		} else if ((*cons)[i]->right_node == n) {
			getConnectorsToRemove(&edge_to_remove, (*cons)[i], CD_LEFT);
		} 
		removeEdge(&edge_to_remove);
	}
}

void GameLogic::removeEdge(vector<Entity*>* edge_to_remove) {
	
	for (int i = 0; i < (*edge_to_remove).size(); i++) {

		if ((*edge_to_remove)[i]->group == group_hub) {
			destroyNode(static_cast<Node*>((*edge_to_remove)[i]));
		} 
		else if ((*edge_to_remove)[i]->group == group_connector) {
			destroyConnector(static_cast<Connector*>((*edge_to_remove)[i]));
		}
	}
	(*edge_to_remove).clear();
}

void GameLogic::destroyNode(Node* n) {
	// if you are deleting the selected node remove the reference
	if (n == n->owner->selected_node) {
		n->owner->selected_node = 0;
	}

	// if firing a surge need to null the pointers to avoid consequetive damage occurences causing memory errors
	if (eActionSelection::AS_SURGE_BOMB) {
		if (n == left_node_surge) {
			left_node_surge = 0;
		}
		else if (n == right_node_surge) {
			right_node_surge = 0;
		}
	}

	// remove collisions for this object so further collisions dont occur before entity removed
	setFixture(n, eNode, eNoCollide);

	if (n->sub_group == subgroup_hub) {
		out_audio_events.push_back(eAudioEvents::AE_HUB_DESTROYED);
		n->owner->nodes.erase(std::remove(n->owner->nodes.begin(), n->owner->nodes.end(), n), n->owner->nodes.end());
	}
	else if (n->sub_group == subgroup_hub_resource) {
		out_audio_events.push_back(eAudioEvents::AE_POWERDOWN_RESOURCE); 
		n->owner->resource_nodes.erase(std::remove(n->owner->resource_nodes.begin(), n->owner->resource_nodes.end(), n), n->owner->resource_nodes.end());
		// if powered on ... nuke
		if (static_cast<NodeHubResource*>(n)->is_powered_on) {
			createExplosion(constructVector(n->getPhysicsObject()->getPos(), explosion_z), subgroup_bomb, mesh_ex_energy, ex_energy_radius, ex_energy_lifetime, ex_energy_damage);
		}
		detachResource(static_cast<NodeHubResource*>(n));
	}
	else if (n->sub_group == subgroup_hub_shield) {
		// if the shield hub has a shield... remove it
		if (static_cast<NodeHubShield*>(n)->shield != 0) {
			// remove the attached shield
			gm->markToDelete(static_cast<NodeHubShield*>(n)->shield);
		}
		out_audio_events.push_back(eAudioEvents::AE_HUB_DESTROYED);
		n->owner->shield_nodes.erase(std::remove(n->owner->shield_nodes.begin(), n->owner->shield_nodes.end(), n), n->owner->shield_nodes.end());
	}

	// remove the ndoes edges
	removeEdges(n);
	//delete reference in nodes
	n->owner->nodes.erase(std::remove(n->owner->nodes.begin(), n->owner->nodes.end(), n), n->owner->nodes.end());
	// then schedule the deletion with the gm
	gm->markToDelete(n);
	// create a game event to say a node has been destroyed. The event will check whether there are remaining nodes
	in_game_events.push_back(eGameEvents::GE_NODE_DESTROYED);
	
}

void GameLogic::createExplosion(Vector3 pos, string subgroup, string mesh, float ex_radius, int lifetime, int damage) {
	
	// if the projectile didnt collide with a shield
	if (!shield_collision_flag) {

		// create an explosion 
		Explosion* e = new Explosion("bomb_ex", "", group_explosion, subgroup, pos,
			gm->iom->findMesh(mesh), gm->iom->findShader("basic"), gm->iom->findTexture("rust"), true, true,
			true, true, 0, ex_radius, world, 0.5, 1.0, lifetime, damage);
	
		// set its fixture so it collides
		setFixture(e, eExplosion, eNode | eShield);

		// special case for surge as the initial blast collides with connectors
		if (AS_SURGE_BOMB) {
			if (subgroup == subgroup_surge_initial) {
				setFixture(e, eExplosion, eNode | eConnector);
			}
		}
		// add to record of current explosion
		explosions.push_back(e);
		// tell the gm about it
		gm->addEntity(e);

		// special case for surge explosions
		if (subgroup == subgroup_surge ) {
			out_audio_events.push_back(eAudioEvents::AE_SURGE_EXPLOSION);
			e->getPhysicsObject()->body->SetActive(false); // explosions from surge dont damage in the usual way
		}
		else {
			if (subgroup == subgroup_repair) {
				out_audio_events.push_back(eAudioEvents::AE_REPAIR);
			} else {
				out_audio_events.push_back(eAudioEvents::AE_EXPLOSION_BOMB);
			}
			// change thte gamesate to wait whilst exploding
			game_state = eGameState::GS_EXPLODING;
		}
	}
	else {
		out_audio_events.push_back(eAudioEvents::AE_SHIELD_COLLISION);
		moveCamera(static_cast<Node*>(active_player->selected_node));
	}
}
void GameLogic::setFixture(Entity* e, uint16 category_bits, uint16 mask_bits) {
	
	//get the existing filter
	b2Filter filter = e->getPhysicsObject()->body->GetFixtureList()->GetFilterData();
	filter.categoryBits = category_bits;
	filter.maskBits = mask_bits;
	e->getPhysicsObject()->body->GetFixtureList()->SetFilterData(filter);
	
}

// adjust whether an entity is rendered or colldable
void GameLogic::editEntity(string name, string parent, bool is_collidable, bool is_renderable) {
	
	Entity* e = gm->getEntityByName(name, parent);
	e->is_renderable = is_renderable;
	e->is_physical = is_collidable;

}

Connector* GameLogic::placeConnector(Vector3 pos, string mesh, float radius, float health, float cost) {
	out_audio_events.push_back(eAudioEvents::AE_CONNECTOR_PLACED);
	Connector* connector = new Connector("", "", group_connector, "", pos,
		gm->iom->findMesh(active_player->player_mesh + mesh), gm->iom->findShader("basic"), gm->iom->findTexture("rust"), true, true,
		true, true, ppm, radius, world, 0.5, 1.0, health, active_player, cost);

	// place it
	gm->addEntity(connector);
	
	// set its fixture so it collides
	setFixture(connector, eConnector, eConnector | eDisablers | eNode );

	// change state to inactive ... activated at a later stage (need to perform the collision operations in a certain order)
	connector->getPhysicsObject()->body->SetActive(false);

	return connector;
}

void GameLogic::destroyUnconnectedNodes() {
	// loop throgh and check each players nodes
	for (int i = 0; i < p1->nodes.size(); i++) {
		bool connected = ConnectionManager::isNodeConnected(p1->nodes[i]);
		if (!connected && p1->nodes.size() > 1) {
			destroyNode(p1->nodes[i]);
			return;
		}
	}
	for (int i = 0; i < p2->nodes.size(); i++) {
		bool connected = ConnectionManager::isNodeConnected(p2->nodes[i]);
		if (!connected && p2->nodes.size() > 1) {
			destroyNode(p2->nodes[i]);
			return;
		}
	}
}

Vector3 GameLogic::constructVector(Vector3 pos, float z) {
	return Vector3(pos.x, pos.y, z);
}

/*****************************************************
HANDLE STATES
******************************************************/

// states occur over multiple frames, this determines action that occur because of this
void GameLogic::handleStates() {

	// declare vars outside switch
	bool live = false; // used for explosion -- determine whether any remaining
	float current_charge;
	Explosion*e;
	Shield* shield;
	b2Vec2 contact_pos;
	float vel;

	switch (game_state) {
	case eGameState::GS_PLAYING:
		world->SetAllowSleeping(true);
		collision_flag = false;
		activation_collision_flag = false;
		repair_flag = false;
		break;

	case eGameState::GS_FIRING:
		cam->SetPosition(constructVector(fired_entity->getPhysicsObject()->getPos(), camera_z));

		if (!isAwake(fired_entity)) {
			game_state = eGameState::GS_CONTACT;
			world->SetAllowSleeping(false);
		}

		else if (fired_entity->group == group_hub) {

			Vector3 start = active_player->selected_node->getPhysicsObject()->getPos();
			Vector3 current_pos = fired_entity->getPhysicsObject()->getPos();
			float length = (current_pos - start).Length();
		
			// if not too close to start
			if (length > con_placement_min) {
				// if not too close to the end
				if (fired_entity->getPhysicsObject()->body->GetLinearVelocity().Length() > min_con_velocity) {

					// if the distance is greater than connector spread -- handles spread and stopping placement too close to start
					if ((length - con_last_placement) > con_spread) {
						// create a connector
						Connector* c = placeConnector(constructVector(current_pos, connector_z), mesh_connector, con_radius, con_health, con_cost);

						// first placement
						if (con_last_placement == 0) {
							c->left_node = ((NodeHub*)active_player->selected_node);
							// add this nodes to the hub edges
							ConnectionManager::addConnector((NodeHub*)active_player->selected_node, c);
						}
						else {
							// set the connectors left pointer to the last connection
							c->left = last_con_ptr;
							// set the left connectors right pointer to this connector
							c->left->right = c;
						}
						// set the last placement after everything else
						con_last_placement = length;
						last_con_ptr = c;
					}
				}
				else if (last_con_ptr) {
					// finished placing nodes -- add the last node to the destination and set its connecting pointer
					last_con_ptr->right_node = (NodeHub*)fired_entity;
					ConnectionManager::addConnector((NodeHub*)fired_entity, last_con_ptr);
					last_con_ptr = NULL;
					con_last_placement = 0.0;
				}
			}
		}
		break;


	case eGameState::GS_SURGE:
		// determines whether explions are still occuring
		live = false;
		// wait between destroying connections
		Sleep(surge_lag);

		// reduce the explosions lifeteime
		for (int i = 0; i < explosions.size(); i++) {
			if (explosions[i] != 0) { // if explosion not expired
				if (explosions[i]->current_life > 0) {
					explosions[i]->current_life--;
					live = true;
				}
				else {
					// mark for removal
					gm->markToDelete(explosions[i]);
					// null the reference
					explosions[i] = 0;

				}	
			}
		}

		// if a left node has been reached but not yet damaged
		if (left_node_surge != 0 && !left_surged) {
			createExplosion(constructVector(left_node_surge->getPhysicsObject()->getPos(), explosion_z), subgroup_surge, mesh_ex_surge, ex_surge_radius, ex_surge_lifetime, 0);
			left_surged = true;

			if (current_surge_damage < surge_min_damage) {
				surge_left_damage = surge_min_damage;
			} else {
				// record the left damage caused ... based on the numbers of nodes its passed through
				surge_left_damage = current_surge_damage; 
			}
		} 

		// if a right node has been reached but not yet damaged
		if (right_node_surge != 0 && ! right_surged) {
			createExplosion(constructVector(right_node_surge->getPhysicsObject()->getPos(), explosion_z), subgroup_surge, mesh_ex_surge, ex_surge_radius, ex_surge_lifetime, 0);
			right_surged = true;
		
			if (current_surge_damage < surge_min_damage) {
				surge_right_damage = surge_min_damage;
			}
			else {
				// record the right damage caused ... based on the numbers of nodes its passed through
				surge_right_damage = current_surge_damage; 
			}
		}

		// go left if any chain remaining
		if (surge_left != 0) {
			// does a left node exist? if not,  found end of chain, mark the left node
			if (surge_left -> left != 0) {
				surge_left = surge_left->left; 
				createExplosion(constructVector(surge_left->getPhysicsObject()->getPos(), explosion_z), subgroup_surge, mesh_ex_surge, ex_surge_radius, ex_surge_lifetime, 0);

			} else {
				left_node_surge = surge_left->left_node;
				surge_left = 0; // stops the algo loking further through chain
			}
		}

		// go right if there is any chain remaining
		if (surge_right != 0) {
			// does a right node exist? if not,  found end of chain, mark the right node
			if (surge_right->right != 0) {
				surge_right = surge_right->right;
				createExplosion(constructVector(surge_right->getPhysicsObject()->getPos(), explosion_z), subgroup_surge, mesh_ex_surge, ex_surge_radius, ex_surge_lifetime, 0);

			}
			else {
				right_node_surge = surge_right->right_node;
				surge_right = 0; // stops the algo loking further through chain
			}
		}

		// determines final dmaage based on the length of the surge
		current_surge_damage = current_surge_damage - surge_deterioration_speed;

		// if left and right have both been surged and no explosions remaining
		if (left_surged && right_surged && !live) {
		
			// clear the explosions vector
			explosions.clear();
			// damage the left and right nodes
			applyDamage(left_node_surge, surge_left_damage);
			// the right node may have been destroyed in the process of damaging the left node. Need to check it still exists
			if (right_node_surge != 0) {
				applyDamage(right_node_surge, surge_right_damage);
			}

			// move the camera back
			moveCamera(static_cast<Node*>(active_player->selected_node));
		}
		break;


	case eGameState::GS_DESTROYING_CONNECTORS:
		// wait between destroying connections
		Sleep(wait);	
		// check left and
		if (destroy_left) {
			destroyConnector(destroy_left); // scheduled
			if (destroy_left->left) {
				destroy_left = destroy_left->left;
			}
			else {
				// remove the edge from the connecting node
				ConnectionManager::removeConnector(destroy_left->left_node, destroy_left);
				// if this is not a hub it can only have 1 connection therefore must be destroyed
				if (destroy_left->left_node->sub_group != subgroup_hub) {
					destroyNode(destroy_left->left_node);
				}
				// indicates no more to destroy in this direction 
				destroy_left = 0;
			}
		}
		
		// check right
		if (destroy_right) {
			destroyConnector(destroy_right); // scheduled
			if (destroy_right->right) {
				destroy_right = destroy_right->right;
			}
			else {
				// remove the edge from the connecting node

				ConnectionManager::removeConnector(destroy_right->right_node, destroy_right);
				// if this is not a hub it can only have 1 connection therefore must be destroyed
				if (destroy_right->right_node->sub_group != subgroup_hub) {
					destroyNode(destroy_right->right_node);
				}
				// indicates no more to destroy in this direction 
				destroy_right = 0;
			}
		}



		if (destroy_left == 0 && destroy_right == 0) {
			
			// reset the collision flag
			collision_flag = false;
			//finished -- check nodes
			destroyUnconnectedNodes();
			// move the camera
			moveCamera(static_cast<Node*>(active_player->selected_node));;
		}


		break;
	case eGameState::GS_FIRING_CLUSTERS:
		
		// reposition the camera to follow the specified cluster...
		if (cluster_ptr_to_follow != nullptr) {
			cam->SetPosition(constructVector(cluster_ptr_to_follow->getPhysicsObject()->getPos(), camera_z));
		}

		if (cluster_counter == cluster_timing_arr[cluster_index]) {
			// fire a new bomb
			fired_entity = new Bomb("c"+cluster_index, "", group_bomb, "", constructVector(active_player->selected_node->getPhysicsObject()->getPos(), bomb_z),
				gm->iom->findMesh(mesh_bomb_cluster), gm->iom->findShader("basic"), gm->iom->findTexture("rust"), true, true,
				true, true, 0, bomb_cluster_radius, world, 0.5, 1.0, bomb_cluster_cost);
		
			fired_entity->getPhysicsObject()->body->SetLinearDamping(damping);
			setFixture(fired_entity, eProjectile, eShield);
			gm->addEntity(fired_entity);
			vec_clusters.push_back(static_cast<Bomb*>(fired_entity));

			//fire - the differing trajectories are altered inside the setTrajectory function
			fireWeapon(fired_entity);
			
			// get the cluster to follow
			if (clusters_fired == cluster_to_follow) {
				cluster_ptr_to_follow = fired_entity;
			}

			// increment after following
			clusters_fired++;

			// incremenent the index if not at the end
			
			if (cluster_index != cluster_count-1) {
				cluster_index++;
			} else {
				all_clusters_fired = true;
			}
		}

		for (int i = 0; i < vec_clusters.size(); i++) {
				// if one of the bombs has stopped moving
			if (!isAwake(vec_clusters[i])) {
				// assign null to stop following when the projectile is removed
				if (vec_clusters[i] == cluster_ptr_to_follow) {
					cluster_ptr_to_follow = nullptr;
				}
				out_audio_events.push_back(eAudioEvents::AE_EXPLOSION_BOMB);
				// create an explosion 
				e = new Explosion("bomb_ex", "", group_explosion, "", constructVector(vec_clusters[i]->getPhysicsObject()->getPos(), explosion_z),
					gm->iom->findMesh(mesh_explosion_cluster), gm->iom->findShader("basic"), gm->iom->findTexture("rust"), true, true,
					true, true, 0, explosion_cluster_radius, world, 0.5, 1.0, explosion_cluster_lifetime, explosion_cluster_damage);
				// set its fixture so it collides
				setFixture(e, eExplosion, eNode | eShield);
				// add to record of current explosion
				explosions.push_back(e);
				// tell the gm about it
				gm->addEntity(e);
				// delete the projectile
				gm->markToDelete(vec_clusters[i]);
				// remove the record of projectile
				vec_clusters.erase(std::remove(vec_clusters.begin(), vec_clusters.end(), vec_clusters[i]), vec_clusters.end()); // clear from vector -- leave here

			}
		}
		
		// check the current explosion and reduce their lifetime, delete if expired
		for (int i = 0; i < explosions.size(); i++) {
			if (explosions[i]->current_life > 0) {
				explosions[i]->current_life--;
			}
			else if (explosions[i]->current_life == 0) {
				gm->markToDelete(explosions[i]);
				explosions.erase(std::remove(explosions.begin(), explosions.end(), explosions[i]), explosions.end()); // clear from vector -- leave here
			}
			break;
		}
		// increment the counter to determine whether the next cluster is fired
		cluster_counter--;

		// if all clusters have been fired and there are no current explosion or projectiles - continue playing.
		if (explosions.size() == 0 && all_clusters_fired && vec_clusters.size() == 0) {
			moveCamera(static_cast<Node*>(active_player->selected_node));;
		}

		break;



	case eGameState::GS_CONTACT:

		switch (action) {
		case GameLogic::AS_HUB:
			setFixture(fired_entity, eNode, eNode | eExplosion | eBlock | eConnector | eDisablers);
			out_audio_events.push_back(eAudioEvents::AE_POWERUP);
			game_state = eGameState::GS_BUILDING;
			break;

		case GameLogic::AS_RESOURCE_HUB:
			setFixture(fired_entity, eNode, eNode | eExplosion | eBlock | eConnector | eDisablers);
			game_state = eGameState::GS_BUILDING;
			break;

		case GameLogic::AS_SHIELD_HUB:
			setFixture(fired_entity, eNode, eNode | eExplosion | eBlock | eConnector | eDisablers);
			game_state = eGameState::GS_BUILDING;
			break;

		case GameLogic::AS_BOMB:
			// create explosion and mvoe to GS_EXPLODING
			createExplosion(constructVector(fired_entity->getPhysicsObject()->getPos(), explosion_z), subgroup_bomb, mesh_ex_bomb, ex_bomb_radius, ex_bomb_lifetime, ex_bomb_damage);
			
			// remove the bomb entity
			gm->markToDelete(fired_entity);
			fired_entity = 0;
			break;
		case GameLogic::AS_REPAIR:
			// create explosion and mvoe to GS_EXPLODING
			createExplosion(constructVector(fired_entity->getPhysicsObject()->getPos(), explosion_z), subgroup_repair, mesh_ex_repair, ex_repair_radius, ex_repair_lifetime, repair_amount);

			// remove the bomb entityas
			gm->markToDelete(fired_entity);
			fired_entity = 0;
			break;
		case GameLogic::AS_SURGE_BOMB:
			out_audio_events.push_back(eAudioEvents::AE_SURGE_EXPLOSION_INIT);

			// create explosion and mvoe to GS_EXPLODING
			createExplosion(constructVector(fired_entity->getPhysicsObject()->getPos(), explosion_z), subgroup_surge_initial, mesh_ex_surge, ex_surge_radius, ex_surge_lifetime, surge_max_damage);

			// remove the bomb entity
			gm->markToDelete(fired_entity);
			fired_entity = 0;
			break;
		
		}

		break;

	case eGameState::GS_COLLIDING:
		if (c_step == c_steps) { // give chance to register collisions before moving on
			// if there werent any collisions during the collisions step period
			if (!collision_flag) { // 0 would mean there was a collison and this entity has been removed
				
				
				activating_connector = ConnectionManager::getLastConnection(static_cast<Node*>(fired_entity));
				// start activating the connectors
				game_state = GS_ACTIVATING_CONNECTORS;
			} else {
				// move the cam back
				c_step = 0;
				moveCamera(static_cast<Node*>(active_player->selected_node));
			}
		}
		// increment the frame counter
		c_step++;
		break;

	case eGameState::GS_ACTIVATING_CONNECTORS:
		if (activation_collision_flag) {
			// end activating the nodes - a collision occured
			moveCamera(static_cast<Node*>(active_player->selected_node));
			break;
		}
		// activate the current connector
		activating_connector->getPhysicsObject()->body->SetActive(true);
		// if there are any connecotrs left in the chain
		if (activating_connector->left) {
			// set the connector to activate during the next frame
			activating_connector = activating_connector->left;
		}
		else {
			c_step = 0; // reset the step
			// finished -- move the camera back and return to playing gamestate
			moveCamera(static_cast<Node*>(active_player->selected_node));
		}
		
		break;

	case eGameState::GS_BUILDING:
		// active the hub so collisions occur

		if (fired_entity != 0) { // 0 would mean the hub collided with something ... so dont activate the shield

			switch (action) {
			case GameLogic::AS_SHIELD_HUB:
			
				shield = new Shield(getName() + "s", "", group_shield, "", constructVector(fired_entity->getPhysicsObject()->getPos(), -4.0),
					gm->iom->findMesh(mesh_shield), gm->iom->findShader("basic"), gm->iom->findTexture("rust"), true, true,
					true, true, ppm, shield_radius, world, 0.5, 1.0, shield_power, active_player);
				
				((NodeHubShield*)fired_entity)->shield = shield;
				setFixture(shield, eShield, eProjectile | eExplosion );
				out_audio_events.push_back(eAudioEvents::AE_SHIELD_ON);
				gm->addEntity(shield);
				break;
			}
		}
		
		// give collisions a chace to occur before moving on
		c_step = 0;
		assert(c_steps > 2); // needs to be atleast a cpl of frames for this to work 
		game_state = GS_COLLIDING;

		break;

	case eGameState::GS_EXPLODING:
		live = false;
		
		for (int i = 0; i < explosions.size(); i++)
		{
			if (explosions[i] != 0) { // if explosion not expired
				// reduce the explosions lifeteime
				if (explosions[i]->current_life > 0) {
					explosions[i]->current_life--;
					live = true;
				} else {
					// if 0 delete the explosion 
					gm->markToDelete(explosions[i]);
					// null the reference
					explosions[i] = 0;
				}
				break;
			}
		}
		// if all explsions dead
		if (!live){
			explosions.clear();
 			moveCamera(static_cast<Node*>(active_player->selected_node));
		}
		break;

	case eGameState::GS_CHARGING:
		
		duration<double> current = system_clock::now() - start;
		current_charge = (current.count() * charge_speed + initial_charge);
		
		if (!fully_charged) {
			if (current_charge >= charge_arr[current_charge_index]) {
				current_charge_index++;
				switch (current_charge_index) {
				case 1:
					out_audio_events.push_back(eAudioEvents::AE_CHARGE_2);
					break;
				case 2:
					out_audio_events.push_back(eAudioEvents::AE_CHARGE_2);
					break;
				case 3:
					out_audio_events.push_back(eAudioEvents::AE_CHARGE_2);
					break;
				case 4:
					out_audio_events.push_back(eAudioEvents::AE_CHARGE_FULL);
					fully_charged = true;
					break;
				}
			}
		}

		if (!charging) {
			end = std::chrono::system_clock::now();
			elapsed_seconds = end - start;
			//cout << "elapsed seconds: " << elapsed_seconds.count() << endl;
			launch();
			fully_charged = false;
			current_charge_index = 0;

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
			// remove the refernence to fired entity
			fired_entity = 0;
			game_state = eGameState::GS_PLAYING;
		}
		break;
	}
}

void GameLogic::moveCamera(Node* n) {

	Vector3 target;
	// check that the node exists.. function may be passing a users selected node which may have been deleted
	if (n == 0) {
		active_player->selected_node = active_player->nodes[0];
		target = constructVector(active_player->selected_node->getPhysicsObject()->getPos(), camera_z);
	}
	else {
		target = constructVector(n->getPhysicsObject()->getPos(), camera_z);
	}
	
	// set the pointer to new pos
	setPointer();
	// reset the step counter
	step_counter = camera_steps;
	target_pos = Vector2(target.x, target.y);
	// calculate the direction and then divide it by the number of steps (frames) to issue the cam moveennt over
	direction_step = target_pos - Vector2(cam->GetPosition().x, cam->GetPosition().y);
	direction_step = Vector2(direction_step.x / camera_steps, direction_step.y / camera_steps);
	// change the game state whilst hte camera is moving to lock out other actions
	game_state = eGameState::GS_CAMERA_MOVING;
}

Node* GameLogic::findPreviousNode() {
	// find the index of selected node
	for (int i = 0; i < active_player->nodes.size(); i++) {
		if (active_player->nodes[i] == active_player->selected_node) {
			if (i == 0) {
				return active_player->nodes[active_player->nodes.size()-1];
			}
			return active_player->nodes[i-1];
		}
	}
}

Node* GameLogic::findNextNode() {
	// find the index of selected node
	for (int i = 0; i < active_player->nodes.size(); i++) {
		if (active_player->nodes[i] == active_player->selected_node) {
			return active_player->nodes[(i + 1) % (active_player->nodes.size())];
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
	case eActionSelection::AS_SHIELD_HUB:
		cost = shield_hub_cost;
		break;
	case eActionSelection::AS_RESOURCE_HUB:
		cost = resource_hub_cost;
		break;
	case eActionSelection::AS_CLUSTER:
		cost = bomb_cluster_cost;
		break;
	case eActionSelection::AS_SURGE_BOMB:
		cost = surge_cost;
		break;
	case eActionSelection::AS_REPAIR:
		cost = repair_cost;
		break;
	}

	if (cost <= active_player->current_resource) {
		active_player->current_resource = active_player->current_resource - cost;
		cout << "-> Resources: " << active_player->current_resource << "/" << active_player->total_resource << endl;
		return true;
	}
	return false;
}

/*****************************************************
HANDLE EVENTS
******************************************************/

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
				cout << endl << " GAME OVER: Player 2 WINS!!!!!!" << endl;
				game_state = eGameState::GS_GAME_END;
				out_audio_events.push_back(eAudioEvents::AE_END);
			}
			if (p2->nodes.size() == 0) {
				cout << endl << "GAME OVER: Player 1 WINS!!!!!!" << endl;
				game_state = eGameState::GS_GAME_END;
				out_audio_events.push_back(eAudioEvents::AE_END);
			}
			break;
		}
	}
	// game events exist in game logic so need to be cleared here... unlike other events
	in_game_events.clear();



	// handle input events
	for (int i = 0; i < in_input_events.size(); i++) {
		
 		switch (in_input_events[i]) {
	
		case eInputEvents::IE_KEY_D:
			if (game_state == eGameState::GS_PLAYING) {
				active_player->selected_node = findNextNode();
				moveCamera(static_cast<Node*>(active_player->selected_node));
			}
			break;
		case eInputEvents::IE_KEY_A:
			if (game_state == eGameState::GS_PLAYING) {
				active_player->selected_node = findPreviousNode();
				moveCamera(static_cast<Node*>(active_player->selected_node));
			}
			break;

		case eInputEvents::IE_ENTER:
			if (game_state == eGameState::GS_PLAYING) {
				out_audio_events.push_back(eAudioEvents::AE_TURN_SWAP);
				endTurn();
			}
			break;
		case eInputEvents::IE_KEY_TAB:
			if (game_state == eGameState::GS_PLAYING) {
				showInstructions();
			}
			break;
		case eInputEvents::IE_SPACE:
			
			if (game_state == eGameState::GS_PLAYING) {
				if (sufficientResource()) {
					game_state = eGameState::GS_CHARGING;
					start = std::chrono::system_clock::now();
				}
				else {
					if (!(Keyboard::KeyHeld(KeyboardKeys(KEY_SPACE)))) {

						cout << "--> Insufficient Resource" << endl;
						out_audio_events.push_back(eAudioEvents::AE_INSUF_RESOURCE);
					}
				}
			}
			if (game_state == eGameState::GS_CHARGING) {
				charging = true;
			}
			break;
		case eInputEvents::IE_LEFT:
			if (game_state == eGameState::GS_PLAYING | game_state == eGameState::GS_FIRING_CLUSTERS) {
				adjustDirection(eInputEvents::IE_LEFT);
				setPointer();
			}
			break;
		case eInputEvents::IE_RIGHT:
			if (game_state == eGameState::GS_PLAYING | game_state == eGameState::GS_FIRING_CLUSTERS) {
				adjustDirection(eInputEvents::IE_RIGHT);
				setPointer();
			}
			break;
		
		case eInputEvents::IE_PAD1:
			if (game_state == eGameState::GS_PLAYING) {
				out_audio_events.push_back(eAudioEvents::AE_SELECT);
				action = eActionSelection::AS_HUB;
				cout << "--> Selected: HUB, Cost: " << hub_cost << endl;
			}
			break;

		case eInputEvents::IE_PAD2:
			if (game_state == eGameState::GS_PLAYING) {
				out_audio_events.push_back(eAudioEvents::AE_SELECT);
				action = eActionSelection::AS_RESOURCE_HUB;
				cout << "--> Selected: RESOURCE HUB, Cost: " << resource_hub_cost << endl;
			}
			break;
		case eInputEvents::IE_PAD3:
			if (game_state == eGameState::GS_PLAYING) {
				out_audio_events.push_back(eAudioEvents::AE_SELECT);
				action = eActionSelection::AS_SHIELD_HUB;
				cout << "--> Selected: SHIELD HUB, Cost: " << shield_hub_cost << endl;
			}
			break;
		case eInputEvents::IE_PAD4:
			if (game_state == eGameState::GS_PLAYING) {
				out_audio_events.push_back(eAudioEvents::AE_SELECT);
				action = eActionSelection::AS_BOMB;
				cout << "--> Selected: BOMB, Cost: " << bomb_cost << ", Damage: " << explosion_cluster_damage << endl;
			}
			break;
		case eInputEvents::IE_PAD5:
			if (game_state == eGameState::GS_PLAYING) {
				out_audio_events.push_back(eAudioEvents::AE_SELECT);
				action = eActionSelection::AS_CLUSTER;
				cout << "--> Selected: CLUSTER BOMB, Cost: " << bomb_cluster_cost << ", Damage: " << explosion_cluster_damage << " per cluster" << endl;
			}
			break;
		case eInputEvents::IE_PAD6:
			if (game_state == eGameState::GS_PLAYING) {
				out_audio_events.push_back(eAudioEvents::AE_SELECT);
				action = eActionSelection::AS_SURGE_BOMB;
				cout << "--> Selected: SURGE BOMB, Cost: " << surge_cost << ", Damage: " << surge_min_damage << " - " << surge_max_damage << " per spike" << endl;
			}
			break;
		case eInputEvents::IE_KEY_ADD:
			if (game_state == eGameState::GS_PLAYING) {
				out_audio_events.push_back(eAudioEvents::AE_SELECT);
				action = eActionSelection::AS_REPAIR;
				cout << "--> Selected: REPAIR, Cost: " << repair_cost << ", Repair: " << repair_amount << endl;
			}
			break;
		}
	}
}

// adjust the direction of the active player. Used to determien which wya to fire or move
void GameLogic::adjustDirection(eInputEvents dir) {
	switch (dir) {
	case GameLogic::IE_LEFT:
		active_player->selected_node->getPhysicsObject()->body->SetAngularVelocity(ROTATION);
		break;
	case GameLogic::IE_RIGHT:
		active_player->selected_node->getPhysicsObject()->body->SetAngularVelocity(-ROTATION);
		break;
	}
}

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

	//check nodes hasnt been detroyed
	if(active_player->selected_node == 0) {
		active_player->selected_node = active_player->nodes[0];
	}
	//powerup shields
	powerupShields(active_player);
	// move the pointer
	setPointer();
	// reset the resource
	active_player->current_resource = active_player->total_resource;
	// reset the step counter
	step_counter = camera_steps;
	// move the camera to new position - game state will change once moved
	moveCamera(static_cast<Node*>(active_player->selected_node));

	string player;
	if (active_player == p1) {
		player = "Player 1";
	}
	else {
		player = "Player 2";
	}

	cout << "END TURN" << endl << endl;
	cout << "Turn: " << player << endl;
	cout << "-> Resources: " << active_player->current_resource << "/" << active_player->total_resource << ", Active Nodes: " << active_player->nodes.size() << endl;
}

void GameLogic::powerupShields(Player* p) {
	// power up any of the players shields
	bool powered = false;
	for (int i = 0; i < p->shield_nodes.size(); i++) {
		if (p->shield_nodes[i]->shield) {

			if (p->shield_nodes[i]->shield->current_power <= 0) {
				setFixture(p->shield_nodes[i]->shield, eShield, eProjectile | eExplosion);
				p->shield_nodes[i]->shield->is_renderable = true;
				// only make th epowered sound once
				if (!powered) {
					out_audio_events.push_back(eAudioEvents::AE_SHIELD_ON);
				}
				powered = true;
			}
			p->shield_nodes[i]->shield->current_power = p->shield_nodes[i]->shield->shield_power;
		}
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
