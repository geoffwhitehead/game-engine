#include "ContactListener.h"


#define explosion_group "explosions"
#define hub_group "hubs"
#define bomb_group "bombs"

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
		if (static_cast<NodeHub*>(bodyDataA)->group == hub_group) {
			if (static_cast<Explosion*>(bodyDataB)->group == explosion_group) {
				gl->in_contact_events.push_back(pair<Entity*, Entity*>(static_cast<NodeHub*>(bodyDataA), static_cast<Explosion*>(bodyDataB)));
			} else
			if (static_cast<NodeHub*>(bodyDataB)->group == hub_group) {
				cout << "hub hub" << endl;
				gl->in_contact_events.push_back(pair<Entity*, Entity*>(static_cast<NodeHub*>(bodyDataA), static_cast<NodeHub*>(bodyDataB)));
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

	if (static_cast<Player*>(bodyDataA)->name == "player_1") {
		if (static_cast<Player*>(bodyDataB)->name == "bomb_explosion") {
			cout << "explosion 1" << endl;
		}
	}
}
