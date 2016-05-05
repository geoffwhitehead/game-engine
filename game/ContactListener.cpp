#include "ContactListener.h"



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
		if (static_cast<NodeHub*>(bodyDataA)) {
			if (static_cast<Explosion*>(bodyDataB)) {
				cout << "hub - explosion" << endl;
			}
		}

		if (static_cast<Explosion*>(bodyDataB)->name == "bomb_explosion") {
			cout << "explosion A" << endl;

			//gl->in_game_events.push_back(GameLogic::eGameEvents::GS_QUIT);
		}

	}

	if (bodyDataB) {
		if (static_cast<Player*>(bodyDataB)->name == "player_1") {
			//cout << "player 1 collided" << endl;
		}
		if (static_cast<Player*>(bodyDataB)->name == "player_2") {
			//cout << "player 1 collided" << endl;
		}
		if (static_cast<Player*>(bodyDataB)->name == "bomb") {
			//cout << "player 1 collided" << endl;
		}

		if (static_cast<Explosion*>(bodyDataB)->name == "bomb_explosion") {
			cout << "explosion B" << endl;

			//gl->in_game_events.push_back(GameLogic::eGameEvents::GS_QUIT);
		}


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
