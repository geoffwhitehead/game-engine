#include "GameLogicManager.h"



GameLogicManager::GameLogicManager()
{
}


GameLogicManager::~GameLogicManager()
{
}

void GameLogicManager::update(float msec) {
	for (int i = 0; i < sub_systems.size(); i++) {
		sub_systems[i]->update(msec);
	}
}

void GameLogicManager::destroy() {

}

void GameLogicManager::init() {
	for (int i = 0; i < sub_systems.size(); i++) {
		sub_systems[i]->init();
	}
}

void GameLogicManager::addSubSystem(SubSystem* ss) {
	sub_systems.push_back(ss);
}
