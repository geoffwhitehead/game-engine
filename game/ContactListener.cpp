#include "ContactListener.h"


//subgroup
const string subgroup_hub_shield = "shield_hub";
const string subgroup_hub_resource = "resource_hub";
const string subgroup_hub = "";
const string subgroup_bomb = "bomb";
const string subgroup_surge = "surge";
const string subgroup_surge_initial = "surge_initial";
const string subgroup_repair = "repair";

// groups
const string group_bomb = "bombs";
const string group_surge_bomb = "surge";
const string group_env_resource = "resource";
const string group_env_block = "impas";
const string group_explosion = "explosions";
const string group_shield = "shields";
const string group_hub = "hubs";
const string group_connector = "connectors";
const string group_repair = "repair";

ContactListener::ContactListener(GameLogic* gl) {
	this->gl = gl;
}


ContactListener::~ContactListener() {

}

// b2ContactListener
// Called when two fixtures begin to touch
void ContactListener::BeginContact(b2Contact* contact) {
	void* bodyDataA = contact->GetFixtureA()->GetBody()->GetUserData();
	void* bodyDataB = contact->GetFixtureB()->GetBody()->GetUserData();

	bool valid_collision = false;
	bool valid_collision_reverse = false;

	Entity* data_a = static_cast<Entity*>(bodyDataA);
	Entity* data_b = static_cast<Entity*>(bodyDataB);

	if (bodyDataA) {
		//cout << "contact" << endl;
		// --HUB
		if (data_a->group == group_hub) {

		    if (data_b->group == group_hub) {
				valid_collision = true;
			}
			else if (data_b->group == group_connector) {
				valid_collision = true;
			}
			else if (data_b->group == group_env_block) {
				valid_collision = true;
			}
			else if (data_b->group == group_env_resource) {
				valid_collision = true;
			}
			else if (data_b->group == group_shield) {
				valid_collision = true;
			}
			else if (data_b->group == group_explosion) {
				valid_collision_reverse = true;
			}
			
		}
		// --ENV ENTITY
		
		else if (data_a->group == group_env_block) {
			if (data_b->group == group_hub) {
				valid_collision_reverse = true;
			}
		}
		// --RESOURCE
		else if (data_a->group == group_env_resource) {
			if (data_b->sub_group == subgroup_hub_resource) {
				valid_collision_reverse = true;
			}
			else if(data_b->group == group_hub) {
				valid_collision_reverse = true;
			}
			else if (data_b->group == group_connector) {
				valid_collision_reverse = true;
			}
		}
		// --SHIELD
		else if (data_a->group == group_shield) {
			if (data_b->group == group_bomb) {
				valid_collision_reverse = true;
			}
			else if (data_b->group == group_surge_bomb) {
				valid_collision_reverse = true;
			}
			else if (data_b->group == group_explosion) {
				valid_collision_reverse = true;
			}
		}

		// -- EXPLOSION
		else if (data_a->group == group_explosion) {
			if (data_b->group == group_hub) {
				valid_collision = true;
			}
			else if (data_b->group == group_shield) {
				valid_collision = true;
			}
			else if (data_b->group == group_connector) {
				valid_collision = true;
			}
		}
		// CONNECTOR
		else if (data_a->group == group_connector) {
			if (data_b->group == group_connector) {
				valid_collision = true;
			}
			else if (data_b->group == group_hub) {
				valid_collision = true;
			}
			else if (data_b->group == group_env_resource) {
				valid_collision = true;
			}
			else if (data_b->sub_group == subgroup_surge_initial) {
				valid_collision_reverse = true;
			}
		}
		// BOMB
		else if (data_a->group == group_bomb) {
			if (data_b->group == group_shield) {
				valid_collision = true;
			}
		}
		// SURGE
		else if (data_a->group == group_surge_bomb) {
			if (data_b->group == group_shield) {
				valid_collision = true;
			}
		}
	}

	assert(!(valid_collision && valid_collision_reverse));

	// send the collision
	if (valid_collision) {
		gl->in_contact_events.push_back(pair<Entity*, Entity*>(data_a, data_b));
	}
	else if (valid_collision_reverse) {
		gl->in_contact_events.push_back(pair<Entity*, Entity*>(data_b, data_a));
	}
}

// Called when two fixtures cease to touch
void ContactListener::EndContact(b2Contact* contact) {
	void* bodyDataA = contact->GetFixtureA()->GetBody()->GetUserData();
	void* bodyDataB = contact->GetFixtureB()->GetBody()->GetUserData();

}
