#include "ContactListener.h"



ContactListener::ContactListener(GameLogic* gl) {
	this->gl = gl;
}


ContactListener::~ContactListener() {

}

// b2ContactListener
// Called when two fixtures begin to touch
void ContactListener::BeginContact(b2Contact* contact) {
	//check if fixture A was a ball
	void* bodyUserData = contact->GetFixtureA()->GetBody()->GetUserData();
	if (bodyUserData) {
		if (static_cast<Player*>(bodyUserData)->name == "player_1") {
			cout << "player 1 collided" << endl;
		}
	}
	//static_cast<Player*>(bodyUserData)->startContact();

	//check if fixture B was a ball
	bodyUserData = contact->GetFixtureB()->GetBody()->GetUserData();
	if (bodyUserData) {
		if (static_cast<Player*>(bodyUserData)->name == "player_2") {
			cout << "player 1 collided with me" << endl;
		}

	}
}

// Called when two fixtures cease to touch
void ContactListener::EndContact(b2Contact* contact) {

}
