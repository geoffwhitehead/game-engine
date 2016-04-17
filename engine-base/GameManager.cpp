#include "GameManager.h"
#include <windows.h>

GameManager::GameManager(float w_x, float w_y)
: window(Window(w_x, w_y)), renderer(Renderer(window)){

}

GameManager::~GameManager(){
	ShutDown();
}
	

// calls destroy on all systems, which in turns call destroy on all their subsystems
void GameManager::ShutDown() {
	cout << "shutdown" << endl;
	// if game has file input, init destroy and delete
	if (iom) {
		iom->destroy();
		delete iom;
		entities.clear();// dont need to destroy objects here - IO manager will delete
	}
	

	for	(vector<SystemManager*>::iterator system = system_managers.begin(); system != system_managers.end(); ++system)
		(*system)->destroy();
	
	Sleep(1000);

}

void GameManager::addEntity(Entity* e){
	entities.push_back(e);
}

void GameManager::addSystemManager(SystemManager* sm) {
	system_managers.push_back(sm);
}

Window* GameManager::getWindow() {
	return &window;
}

vector<Entity*>* GameManager::getEntities() {
	return &entities;
}

// registers an IO class with the game manager to load entities from
void GameManager::addFileInput(IOManager* iom) {
	this->iom = iom;
}

// removes previous level (if exists) ready for loading new level
void GameManager::clear() {
	entities.clear();
	iom->clearLevel();
}

void GameManager::loadLevel(string file_name) {
	
	// must have enabled file input by loading IOManager prior to calling this function
	assert(iom);

	clear();

	iom->load(file_name);

	// register new entities with game manager
	for (int i = 0; i < iom->in_entity.size(); i++) {
		if (iom->in_entity[i]->str_parent == "") { // no parent
			addEntity(iom->in_entity[i]);
		}
		else {
			for (int j = 0; j < iom->in_entity.size(); j++) {
				if (iom->in_entity[j]->name == iom->in_entity[i]->str_parent){ // no parent
					iom->in_entity[j]->addChild(iom->in_entity[i]);
					break;
				}
			}
		}
	}
}

// returns a pointer to an entity that matches params
Entity* GameManager::getEntityByName(string name_to_find, string parent_name) {
	if (parent_name == "") {
		for (int i = 0; i < entities.size(); i++) {
			if (entities[i]->name == name_to_find) {
				return entities[i];
			}
		}
	}
	else {
		for (int i = 0; i < entities.size(); i++) {
			if (entities[i]->name == parent_name) {
				for (int j = 0; j < entities[i]->getChildren().size(); j++) {
					if (entities[i]->getChildren()[j]->name == name_to_find) {
						return entities[i]->getChildren()[j];
					}
				}
			}
		}
	}
	return nullptr;
}

// main game loop
void GameManager::run(){
	
	// initialise systems
	for (vector<SystemManager*>::iterator system = system_managers.begin(); system != system_managers.end(); ++system)
		(*system)->init();

	while (window.UpdateWindow()){
		float msec = window.GetTimer()->GetTimedMS();

		msec *= 2.0f;

		// update systems
		for (vector<SystemManager*>::iterator system = system_managers.begin(); system != system_managers.end(); ++system)
			(*system)->update(msec);

		// update entities
		for (vector<Entity*>::iterator entity = entities.begin(); entity != entities.end(); ++entity) {
			(*entity)->update(msec);
		}

		renderer.ClearBuffers();

		// render entities
		for (vector<Entity*>::iterator entity = entities.begin(); entity != entities.end(); ++entity) {
			(*entity)->render(&renderer);
		}
		
		renderer.SwapBuffers();
	}
	
	// init shutdown performing safe deletion of everything
	ShutDown();

}

