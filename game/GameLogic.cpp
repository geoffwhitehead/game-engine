#include "GameLogic.h"

const Vector3 BAULK_LINE = Vector3(-104.775, 0.0, 5.0);
#define SEMI_RADIUS 29.2f

#define START_POS_YELLOW Vector3(-104.775, -29.21, 5.0)
#define START_POS_GREEN Vector3(-104.775, 29.21, 5.0)
#define START_POS_BROWN Vector3(-104.775, 0.0, 5.0 )
#define START_POS_BLUE Vector3(0.0, 0.0, 5.0 )
#define START_POS_PINK Vector3(88.685, 0.0, 5.0)
#define START_POS_BLACK Vector3(146.685, 0.0, 5.0)

#define VEC_STILL Vector3(0.0,0.0,5.0)

int red_balls_remaining = 1;

//yellow, green, brown, blue, pink, black

GameLogic::GameLogic(GameLogicManager* glm, GameEvents* ge){
	this->ge = ge;
	this->glm = glm;
}



GameLogic::~GameLogic()
{
}

// logic events that occur at the start of the game
void GameLogic::init() {
	ge->game_state = GameEvents::eGameState::GS_PLACE_WHITE;
	ge->ball_on = GameEvents::eBallOn::BO_RED;
	editEntity("white", "table", false, true);
}

void GameLogic::update(float msec) {
	ge->out_clearEvents(GameEvents::ET_LOGIC);
	handleEvents();
	handleStates();
	ge->in_clearEvents(GameEvents::ET_LOGIC);
}

void GameLogic::destroy() {

}

// adjust whether an entity is rendered or colldable
void GameLogic::editEntity(string name, string parent, bool is_collidable, bool is_renderable) {
	
	Entity* e = ge->gm->getEntityByName(name, parent);
	e->is_renderable = is_renderable;
	e->is_collidable = is_collidable;

}

// logic that occurs when a legal ball is potted - fouls are caught earlier
void GameLogic::resolvePot(string name) {
	for (int i = 0; i < ge->out_cols_circle_pocket.size(); i++){
		Entity* e = ge->out_cols_circle_pocket[i].first;
		if (e->sub_group == name || e->name == name) { // use sub group to resolve red pots which are named differently
			e->is_renderable = false;
			e->is_collidable = false;
			if (name == "red") {
				red_balls_remaining--;
			}
		}
	}
}

// reset the ball to its starting position
void GameLogic::resetBall(string name, Vector3 pos) {
	for (int i = 0; i < ge->out_cols_circle_pocket.size(); i++) {
		Entity* e = ge->out_cols_circle_pocket[i].first;
		if (e->name == name) { 
			e->is_renderable = true;
			e->is_collidable = true;
			e->setPos(pos);
			e->setVel(VEC_STILL);
		}
	}
}

// determine actions based on logic events that have been stacked between frame 
void GameLogic::handleEvents() {
	for (int i = 0; i < this->ge->in_logic_events.size(); i++) {
		cout << ge->in_logic_events[i];
		switch (ge->in_logic_events[i]) {
		case GameEvents::eLogicEvents::LE_POT_RED:
			resolvePot("red");
			if (red_balls_remaining == 0) {
				ge->ball_on = GameEvents::eBallOn::BO_YELLOW;
			}
			break;

		case GameEvents::eLogicEvents::LE_POT_YELLOW:
			if (ge->ball_on == GameEvents::eBallOn::BO_YELLOW) {
				resolvePot("yellow");
				ge->ball_on = GameEvents::eBallOn::BO_GREEN;
			}
			else {
				resetBall("yellow", START_POS_YELLOW);
			}
			break;

		case GameEvents::eLogicEvents::LE_POT_GREEN:
			if (ge->ball_on == GameEvents::eBallOn::BO_GREEN) {
				resolvePot("green");
				ge->ball_on = GameEvents::eBallOn::BO_BROWN;
			}
			else {
				resetBall("green", START_POS_GREEN);
			}
			break;

		case GameEvents::eLogicEvents::LE_POT_BROWN:
			if (ge->ball_on == GameEvents::eBallOn::BO_BROWN) {
				resolvePot("brown");
				ge->ball_on = GameEvents::eBallOn::BO_BLUE;
			}
			else {
				resetBall("brown", START_POS_BROWN);
			}
			break;

		case GameEvents::eLogicEvents::LE_POT_BLUE:
			if (ge->ball_on == GameEvents::eBallOn::BO_BLUE) {
				resolvePot("blue");
				ge->ball_on = GameEvents::eBallOn::BO_PINK;
			}
			else {
				resetBall("blue", START_POS_BLUE);
			}
			break;

		case GameEvents::eLogicEvents::LE_POT_PINK:
			if (ge->ball_on == GameEvents::eBallOn::BO_PINK) {
				resolvePot("pink");
				ge->ball_on = GameEvents::eBallOn::BO_BLACK;
			}
			else {
				resetBall("pink", START_POS_PINK);
			}
			break;

		case GameEvents::eLogicEvents::LE_POT_BLACK:
			if (ge->ball_on == GameEvents::eBallOn::BO_BLACK) {
				resolvePot("black");
				ge->out_logic_events.push_back(GameEvents::eLogicEvents::LE_GAMEOVER);
			}
			else {
				resetBall("black", START_POS_BLACK);
			}
			break;

		case GameEvents::eLogicEvents::LE_POT_WHITE:
			ge->game_state = GameEvents::eGameState::GS_PLACE_WHITE;
			editEntity("white", "table", false, true);
			ge->gm->getEntityByName("white", "table")->setVel(VEC_STILL);

			break;

		case GameEvents::eLogicEvents::LE_PLACED_WHITE:
			if (isLegalPosition()){
				ge->game_state = GameEvents::eGameState::GS_PLAYING;
				editEntity("white", "table", true, true);
			}
		}
	}
}

// actions that occur every frame when in a particular state
void GameLogic::handleStates() {
	switch (ge->game_state) {
	case GameEvents::eGameState::GS_PLACE_WHITE:
		Entity* white = ge->gm->getEntityByName("white", "table");
		white->setPos(ge->getMousePos3D());
		break;

	}
}

// used when placing the cue ball, if this check passes the cue ball is placed at this position.
// the position must be in the semi circle at the top of the table
bool GameLogic::isLegalPosition() {
	Vector3 m_pos = ge->getMousePos3D();
	if (m_pos.x < BAULK_LINE.x && (BAULK_LINE - m_pos).Length() < SEMI_RADIUS){
		return true;
	}
	return false;
}