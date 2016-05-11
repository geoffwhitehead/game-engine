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
	NodeHub* n1 = new NodeHub("p1_node_0", "", group_hub, "", p1_start_loc,
		gm->iom->findMesh(p1_mesh + node_hub), gm->iom->findShader("basic"), gm->iom->findTexture("rust"), true, true,
		true, true, ppm, hub_radius, world, 0.5, 1.0, hub_health, p1, hub_cost);
	NodeHub* n2 =  new NodeHub("p2_node_0", "", group_hub, "", p2_start_loc,
		gm->iom->findMesh(p2_mesh + node_hub), gm->iom->findShader("basic"), gm->iom->findTexture("rust"), true, true,
		true, true, ppm, hub_radius, world, 0.5, 1.0, hub_health, p2, hub_cost);
	

	// assign them to aplayer
	p1->nodes.push_back(n1);
	p2->nodes.push_back(n2);
	
	// notify the connection manager
	ConnectionManager::addNode(n1);
	ConnectionManager::addNode(n2);

	// set the starting node fixtures
	setFixture(p1->nodes[0], eNode, eExplosion | eBlock | eConnector);
	setFixture(p2->nodes[0], eNode, eExplosion | eBlock | eConnector);

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
			setFixture(gm->entities[i], eBlock, eNode);
		}
	}
	// set the starting resource
	p1->total_resource = starting_resource;
	p1->current_resource = starting_resource;
	p2->total_resource = starting_resource;
	p2->current_resource = starting_resource;

	// set the player to start first and position the cam accordingly
	active_player = p1;
	cam->SetPosition(Vector3(p1_start_loc.x, p1_start_loc.y, CAM_OFFSET));

	// set the pointer entity properties
	pointer = gm->getEntityByName("pointer", "");
	pointer_mark = gm->getEntityByName("pointer_centre", "");
	setPointer();
	setFixture(pointer, ePointer, eNoCollide);
	setFixture(pointer_mark, ePointer, eNoCollide);
	// cretae the connection manager
	//static ConnectionManager conman;
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

		hub = new NodeHub(getName(), "", group_hub, "", active_player->selected_node->getPhysicsObject()->getPos(),
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

		res_hub = new NodeHubResource(getName(), "", group_hub, subgroup_hub_resource, active_player->selected_node->getPhysicsObject()->getPos(),
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

		shield_hub = new NodeHubShield(getName(), "", group_hub, subgroup_hub_shield, active_player->selected_node->getPhysicsObject()->getPos(),
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
		b1 = new Bomb(getName(), "", group_bomb, "", active_player->selected_node->getPhysicsObject()->getPos(),
			gm->iom->findMesh("mesh_bomb"), gm->iom->findShader("basic"), gm->iom->findTexture("rust"), true, true,
			true, true, 0, bomb_radius, world, 0.5, 1.0, bomb_cost);

		b1->getPhysicsObject()->body->SetLinearDamping(damping);
		setFixture(b1, eProjectile, eShield);
		gm->addEntity(b1);

		fireWeapon(b1);
		fired_entity = b1;
		game_state = GS_FIRING;
		break;

	case GameLogic::AS_CLUSTER:
		game_state = GS_FIRING_CLUSTERS;

		// reset the vars for cluster
		all_clusters_fired = false;
		cluster_counter = cluster_firing_time;
		cluster_index = 0;
		clusters_fired = 0;
		// initialise to nullptr. camera checks for null when following
		cluster_ptr_to_follow = nullptr;
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
	pointer_mark->getPhysicsObject()->body->SetTransform(
		b2Vec2(active_player->selected_node->getPhysicsObject()->getPos().x, 
			active_player->selected_node->getPhysicsObject()->getPos().y), 
		pointer_z
	);
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
	// destroy this initial connector
	destroyConnector(new_con);
	// check left and right and mark those to be destroyed if they exist
	if (new_con->left) {
		destroy_left = new_con->left;
	}
	if (new_con->right) {
		destroy_right = new_con->right;
	}
	// this game state will iterate through all the connectors destroying them
	game_state = eGameState::GS_DESTROYING_CONNECTORS;
}


/*****************************************************
HANDLE COLLISIONS
******************************************************/

void GameLogic::handleCollisions() {
	for (int i = 0; i < in_contact_events.size(); i++){
		cout << "contact in handle collisions" << endl;
		// CONNECTOR

		if (in_contact_events[i].first->group == group_connector) {
			if (in_contact_events[i].second->group == group_connector && !collision_flag) {
				collision_flag = true;
				connectorIntersection(static_cast<Connector*>(in_contact_events[i].first), static_cast<Connector*>(in_contact_events[i].second));
			}
		}
		//EXPLOSION
		if (in_contact_events[i].first->group == group_explosion){
			if (in_contact_events[i].second->group == group_hub) {
				applyDamage(static_cast<Node*>(in_contact_events[i].second), static_cast<Explosion*>(in_contact_events[i].first)->damage);
			}
		}
		// SHIELD
		else if (in_contact_events[i].first->group == group_shield && !(static_cast<Shield*>(in_contact_events[i].first)->owner == active_player)) {
			if (true) {
				if (in_contact_events[i].second->group == group_bomb) {
					in_contact_events[i].second->getPhysicsObject()->body->SetLinearVelocity(b2Vec2(0, 0));
				}
				else if (in_contact_events[i].second->group == group_hub) {
					applyDamage(static_cast<Node*>(in_contact_events[i].second), kill);
					applyDamage(static_cast<Shield*>(in_contact_events[i].first), node_damage);
				}
				else if (in_contact_events[i].second->group == group_explosion) {
					applyDamage(static_cast<Shield*>(in_contact_events[i].first), static_cast<Explosion*>(in_contact_events[i].second)->damage);
				}
			}
		}
		// HUB
		else if (in_contact_events[i].first->group == group_hub){
			// hub connector
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

			// -- hub - hub
			if (in_contact_events[i].second->group == group_hub) {

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
				// hub - explosion
				if (in_contact_events[i].second->group == group_explosion) {
					applyDamage(static_cast<Node*>(in_contact_events[i].first), static_cast<Explosion*>(in_contact_events[i].second)->damage);
				}
				// hub - block
				else if (in_contact_events[i].second->group == group_env_block) {
					applyDamage(static_cast<Node*>(in_contact_events[i].first), kill);
				}
				// hub - resource
				else if (in_contact_events[i].second->group == group_env_resource) {
					applyDamage(static_cast<Node*>(in_contact_events[i].first), kill);
				}
			}
			// -- hub specific
			else if (in_contact_events[i].first->sub_group == subgroup_hub) {
				// hub - explosion
				if (in_contact_events[i].second->group == group_explosion) {
					applyDamage(static_cast<Node*>(in_contact_events[i].first), static_cast<Explosion*>(in_contact_events[i].second)->damage);
				}
				// hub - block
				else if (in_contact_events[i].second->group == group_env_block) {
					applyDamage(static_cast<Node*>(in_contact_events[i].first), kill);
				}
				// hub - resource
				else if (in_contact_events[i].second->group == group_env_resource) {
					applyDamage(static_cast<Node*>(in_contact_events[i].first), kill);
				}
			}
			// -- resource hub specific
			else if (in_contact_events[i].first->sub_group == subgroup_hub_resource) {
				// resource hub - env resource
				if (in_contact_events[i].second->group == group_env_resource) {
					applyResource(static_cast<NodeHubResource*>(in_contact_events[i].first));
				}
				// resource hub - explosion
				else if (in_contact_events[i].second->group == group_explosion) {
					applyDamage(static_cast<NodeHubResource*>(in_contact_events[i].first), static_cast<Explosion*>(in_contact_events[i].second)->damage);
				}
			}
		}
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
	}
}

void GameLogic::detachResource(NodeHubResource* n) {
	// just incase the event occurs on a node already powered down
	if (n->is_powered_on) {
		n->owner->total_resource = active_player->total_resource - n->resource_per_turn;
		n->is_powered_on = false;
	}
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
			destroyNode(n);
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
			createExplosion(n->getPhysicsObject()->getPos(), mesh_ex_energy, ex_energy_radius, ex_energy_lifetime, ex_energy_damage);
		}
		detachResource(static_cast<NodeHubResource*>(n));
	}
	else if (n->sub_group == subgroup_hub_shield) {
		out_audio_events.push_back(eAudioEvents::AE_HUB_DESTROYED);
		// remove the attached shield
		n->owner->shield_nodes.erase(std::remove(n->owner->shield_nodes.begin(), n->owner->shield_nodes.end(), n), n->owner->shield_nodes.end());
		gm->markToDelete(static_cast<NodeHubShield*>(n)->shield);
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

void GameLogic::createExplosion(Vector3 pos, string mesh, float ex_radius, int lifetime, int damage) {
	
	out_audio_events.push_back(eAudioEvents::AE_EXPLOSION_BOMB);
	// create an explosion 
	Explosion* e = new Explosion("bomb_ex", "", group_explosion, "", pos,
		gm->iom->findMesh(mesh), gm->iom->findShader("basic"), gm->iom->findTexture("rust"), true, true,
		true, true, 0, ex_radius, world, 0.5, 1.0, lifetime, damage);
	// set its fixture so it collides
	setFixture(e, eExplosion, eNode | eShield);
	// add to record of current explosion
	explosions.push_back(e);
	// tell the gm about it
	gm->addEntity(e);
	// change thte gamesate to wait whilst exploding
	game_state = eGameState::GS_EXPLODING;
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
	setFixture(connector, eConnector, eConnector | eDisablers | eNode);

	// change state to inactive ... activated at a later stage (need to perform the collision operations in a certain order)
	connector->getPhysicsObject()->body->SetActive(false);

	return connector;
}

void GameLogic::destroyUnconnectedNodes() {
	// loop throgh and check each players nodes
	for (int i = 0; i < p1->nodes.size(); i++) {
		if (!ConnectionManager::isNodeConnected(p1->nodes[i]) && p1->nodes.size() > 1) {
			destroyNode(p1->nodes[i]);
			return;
		}
	}
	for (int i = 0; i < p2->nodes.size(); i++) {
		if (!ConnectionManager::isNodeConnected(p2->nodes[i]) && p2->nodes.size() > 1) {
			destroyNode(p2->nodes[i]);
			return;
		}
	}
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
		break;
	case eGameState::GS_FIRING:
		cam->SetPosition(Vector3(
			fired_entity->getPhysicsObject()->getPos().x
			,fired_entity->getPhysicsObject()->getPos().y, 
			CAM_OFFSET)
		);
		if (!isAwake(fired_entity)) {
			game_state = eGameState::GS_CONTACT;
			world->SetAllowSleeping(false);
		}
		else if (fired_entity->group == group_hub) {

			Vector3 start = active_player->selected_node->getPhysicsObject()->getPos();
			Vector3 current_pos = fired_entity->getPhysicsObject()->getPos();
			float length = (current_pos - start).Length();
			//cout << "length " << length << " vel length " << fired_entity->getPhysicsObject()->body->GetLinearVelocity().Length() << endl;
			// if not too close to start
			if (length > con_placement_min) {
				// if not too close to the end
				if (fired_entity->getPhysicsObject()->body->GetLinearVelocity().Length() > min_con_velocity) {


					//cout << "travelled " << start << " ---" << current_pos << " --- " << " ---- " << length << endl;

					// if the distance is greater than connector spread -- handles spread and stopping placement too close to start
					if ((length - con_last_placement) > con_spread) {
						// create a connector
						Connector* c = placeConnector(current_pos, mesh_connector, con_radius, con_health, con_cost);

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
			moveCamera(active_player->selected_node->getPhysicsObject()->getPos());;
		}


		break;
	case eGameState::GS_FIRING_CLUSTERS:
		
		// reposition the camera to follow the specified cluster...
		if (cluster_ptr_to_follow != nullptr) {
			cam->SetPosition(Vector3(
				cluster_ptr_to_follow->getPhysicsObject()->getPos().x,
				cluster_ptr_to_follow->getPhysicsObject()->getPos().y,
				CAM_OFFSET)
			);
		}

		if (cluster_counter == cluster_timing_arr[cluster_index]) {
			// fire a new bomb
			fired_entity = new Bomb("c"+cluster_index, "", group_bomb, "", active_player->selected_node->getPhysicsObject()->getPos(),
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
				e = new Explosion("bomb_ex", "", group_explosion, "", vec_clusters[i]->getPhysicsObject()->getPos(),
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
			moveCamera(active_player->selected_node->getPhysicsObject()->getPos());;
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
			out_audio_events.push_back(eAudioEvents::AE_EXPLOSION_BOMB);

			// create explosion and mvoe to GS_EXPLODING
			createExplosion(fired_entity->getPhysicsObject()->getPos(), mesh_ex_bomb, ex_bomb_radius, ex_bomb_lifetime, ex_bomb_damage);

			gm->markToDelete(fired_entity);
			// null the pointer
			fired_entity = 0;
			break;

		}

		break;

	case eGameState::GS_COLLIDING:
		if (c_step == c_steps) { // give chance to register collisions before moving on
			// if there werent any collisions during the collisions step period
			if (!collision_flag) { // 0 would mean there was a collison and this entity has been removed
				
				// get the last connection -- could have maintained a refernce after just placing it which is more effecient... i just prefer to excapsulate the game state incase the value was changed
				activating_connector = ConnectionManager::getLastConnection(static_cast<Node*>(fired_entity));
				// start activating the connectors
				game_state = GS_ACTIVATING_CONNECTORS;
			} else {
				// move the cam back
				c_step = 0;
				moveCamera(active_player->selected_node->getPhysicsObject()->getPos());
			}
		}
		// increment the frame counter
		c_step++;
		break;

	case eGameState::GS_ACTIVATING_CONNECTORS:
		if (collision_flag) {
			// end activating the nodes - a collision occured
			moveCamera(active_player->selected_node->getPhysicsObject()->getPos());
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
			moveCamera(active_player->selected_node->getPhysicsObject()->getPos());
		}
		
		break;

	case eGameState::GS_BUILDING:
		// active the hub so collisions occur

		switch (action) {
		case GameLogic::AS_SHIELD_HUB:
			
			shield = new Shield(getName() + "s", "", group_shield, "", Vector3(fired_entity->getPhysicsObject()->getPos().x, 
				fired_entity->getPhysicsObject()->getPos().y, -4.0),
				gm->iom->findMesh(mesh_shield), gm->iom->findShader("basic"), gm->iom->findTexture("rust"), true, true,
				true, true, ppm, shield_radius, world, 0.5, 1.0, shield_power, active_player);
				
			((NodeHubShield*)fired_entity)->shield = shield;
			setFixture(shield, eShield, eProjectile | eExplosion );
			out_audio_events.push_back(eAudioEvents::AE_SHIELD_ON);
			gm->addEntity(shield);
			break;
		}
		
		// give collisions a chace to occur before moving on
		c_step = 0;
		assert(c_steps > 2); // needs to be atleast a cpl of frames for this to work 
		game_state = GS_COLLIDING;

		break;

	case eGameState::GS_EXPLODING:
		
		for (int i = 0; i < explosions.size(); i++)
		{
			if (explosions[i]->current_life > 0) {
				explosions[i]->current_life--;
				live = true;
			}
			else if (explosions[i]->current_life == 0) {
				
				//explosions.erase(std::remove(explosions.begin(), explosions.end(), explosions[i]), explosions.end()); // clear from vector -- leave here
			}
			break;
		}
		if (live == false){

			for (int i = 0; i < explosions.size(); i++) {
				gm->markToDelete(explosions[i]);
			}
			explosions.clear();
			moveCamera(active_player->selected_node->getPhysicsObject()->getPos());
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
					out_audio_events.push_back(eAudioEvents::AE_CHARGE_2);
					break;
				case 5:
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

void GameLogic::moveCamera(Vector3 target) {
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
	}

	if (cost <= active_player->current_resource) {
		active_player->current_resource = active_player->current_resource - cost;
		//cout << "2: " << active_player->current_resource << " " << cost << endl;
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
				cout << "Player 2 WINS!!!!!!" << endl;
				game_state = eGameState::GS_GAME_END;
			}
			if (p2->nodes.size() == 0) {
				cout << "Player 1 WINS!!!!!!" << endl;
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
	
		case eInputEvents::IE_KEY_D:
			if (game_state == eGameState::GS_PLAYING) {
				active_player->selected_node = findNextNode();
				moveCamera(active_player->selected_node->getPhysicsObject()->getPos());
			}
			break;
		case eInputEvents::IE_KEY_A:
			if (game_state == eGameState::GS_PLAYING) {

				active_player->selected_node = findPreviousNode();
				moveCamera(active_player->selected_node->getPhysicsObject()->getPos());
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
				if (sufficientResource()) {
					game_state = eGameState::GS_CHARGING;
					start = std::chrono::system_clock::now();
				}
				else {
					if (!(Keyboard::KeyHeld(KeyboardKeys(KEY_SPACE)))) {
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
			}
			break;

		case eInputEvents::IE_PAD2:
			if (game_state == eGameState::GS_PLAYING) {
				out_audio_events.push_back(eAudioEvents::AE_SELECT);
				action = eActionSelection::AS_RESOURCE_HUB;
			}
			break;
		case eInputEvents::IE_PAD3:
			if (game_state == eGameState::GS_PLAYING) {
				out_audio_events.push_back(eAudioEvents::AE_SELECT);
				action = eActionSelection::AS_SHIELD_HUB;
			}
			break;
		case eInputEvents::IE_PAD4:
			if (game_state == eGameState::GS_PLAYING) {
				out_audio_events.push_back(eAudioEvents::AE_SELECT);
				action = eActionSelection::AS_BOMB;
			}
			break;
		case eInputEvents::IE_PAD5:
			if (game_state == eGameState::GS_PLAYING) {
				out_audio_events.push_back(eAudioEvents::AE_SELECT);
				action = eActionSelection::AS_CLUSTER;
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
	//powerup shields
	powerupShields(active_player);
	// move the pointer
	setPointer();
	// reset the resource
	active_player->current_resource = active_player->total_resource;
	// reset the step counter
	step_counter = camera_steps;
	// find the position of the next node and set it as the active node
	Entity* next_node = active_player->selected_node;

	// move the camera to new position - game state will change once moved
	moveCamera(next_node->getPhysicsObject()->getPos());

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
