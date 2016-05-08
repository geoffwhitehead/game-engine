#include "ContactListener.h"


#define group_explosion "explosions"
#define group_hub "hubs"
#define subgroup_hub_resource "resource_hub"
#define subgroup_hub ""
#define group_bomb "bombs"
#define group_env_resource "resource"
#define group_env_block "impas"


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

	if (bodyDataA) {
		cout << "contact" << endl;
		// --HUB
		if (static_cast<Entity*>(bodyDataA)->group == group_hub) {
			cout << "hub contact" << endl;
			if (static_cast<Entity*>(bodyDataB)->group == group_explosion) {
				gl->in_contact_events.push_back(pair<Entity*, Entity*>(static_cast<Entity*>(bodyDataA), static_cast<Entity*>(bodyDataB)));
			} 
			else if (static_cast<Entity*>(bodyDataB)->group == group_hub) {
				cout << "hub hub" << endl;
				gl->in_contact_events.push_back(pair<Entity*, Entity*>(static_cast<Entity*>(bodyDataA), static_cast<Entity*>(bodyDataB)));
			}
			else if (static_cast<Entity*>(bodyDataB)->group == group_env_block) {
				cout << "hub block!!!!!!!" << endl;
				gl->in_contact_events.push_back(pair<Entity*, Entity*>(static_cast<Entity*>(bodyDataA), static_cast<Entity*>(bodyDataB)));
			}
			else if (static_cast<Entity*>(bodyDataB)->group == group_env_resource) {
				cout << "hub resource!!!!!" << endl;
				gl->in_contact_events.push_back(pair<Entity*, Entity*>(static_cast<Entity*>(bodyDataA), static_cast<Entity*>(bodyDataB)));
			}
		}
		// --ENV ENTITY
		if (static_cast<Entity*>(bodyDataA)->group == group_env_block) {
			if (static_cast<Entity*>(bodyDataB)->group == group_hub) {
				gl->in_contact_events.push_back(pair<Entity*, Entity*>(static_cast<Entity*>(bodyDataB), static_cast<Entity*>(bodyDataA)));
			}
		}
		// --RESOURCE
		if (static_cast<Entity*>(bodyDataA)->group == group_env_resource) {
			if (static_cast<Entity*>(bodyDataB)->sub_group == subgroup_hub) {
				gl->in_contact_events.push_back(pair<Entity*, Entity*>(static_cast<Entity*>(bodyDataB), static_cast<Entity*>(bodyDataA)));
			}
			if (static_cast<Entity*>(bodyDataB)->sub_group == subgroup_hub_resource) {
				gl->in_contact_events.push_back(pair<Entity*, Entity*>(static_cast<Entity*>(bodyDataB), static_cast<Entity*>(bodyDataA)));
			}
		}
	}

	if (bodyDataB) {
		//cout << "something b" << endl;
	}
}

// Called when two fixtures cease to touch
void ContactListener::EndContact(b2Contact* contact) {
	void* bodyDataA = contact->GetFixtureA()->GetBody()->GetUserData();
	void* bodyDataB = contact->GetFixtureB()->GetBody()->GetUserData();

}
