#include "InputManager.h"


InputManager::InputManager()
{
}


InputManager::~InputManager()
{
}

void InputManager::update(float msec) {
	for (int i = 0; i < sub_systems.size(); i++) {
		sub_systems[i]->update(msec);
	}
}

void InputManager::destroy() {

}

void InputManager::init() {
	for (int i = 0; i < sub_systems.size(); i++) {
		sub_systems[i]->init();
	}
}

void InputManager::addSubSystem(SubSystem* ss) {
	sub_systems.push_back(ss);
}