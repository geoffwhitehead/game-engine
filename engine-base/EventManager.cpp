#include "EventManager.h"



EventManager::EventManager()
{
}


EventManager::~EventManager()
{
}

void EventManager::update(float msec) {
	for (int i = 0; i < sub_systems.size(); i++) {
		sub_systems[i]->update(msec);
	}
}

void EventManager::destroy() {

}

void EventManager::init() {
	for (int i = 0; i < sub_systems.size(); i++) {
		sub_systems[i]->init();
	}
}

void EventManager::addSubSystem(SubSystem* ss) {
	sub_systems.push_back(ss);
}