#include "GameManager.h"

GameManager::GameManager(float w_x, float w_y)
: window(Window(w_x, w_y)), renderer(Renderer(window)){

}

GameManager::~GameManager(){

}
	//for (vector<GLuint>::iterator tex = textures.begin(); tex != textures.end(); ++tex)
		//delete (*tex);


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

void GameManager::addFileInput(IOManager* iom) {
	this->iom = iom;
}

void GameManager::loadLevel(string file_name) {
	iom->load(file_name);

	// register entities
	for (auto iterator = iom->in_entity.begin(); iterator != iom->in_entity.end(); iterator++)
		addEntity(*iterator);
}

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

void GameManager::run(){
	
	

	for (vector<SystemManager*>::iterator system = system_managers.begin(); system != system_managers.end(); ++system)
		(*system)->init();

	while (window.UpdateWindow()){
		float msec = window.GetTimer()->GetTimedMS();

		msec *= 2.0f;

		for (vector<SystemManager*>::iterator system = system_managers.begin(); system != system_managers.end(); ++system)
			(*system)->update(msec);

		for (vector<Entity*>::iterator entity = entities.begin(); entity != entities.end(); ++entity) {
			(*entity)->update(msec);
		}

		renderer.ClearBuffers();

		for (vector<Entity*>::iterator entity = entities.begin(); entity != entities.end(); ++entity) {
			(*entity)->render(&renderer);
		}

		renderer.SwapBuffers();
	}
	for (vector<SystemManager*>::iterator system = system_managers.begin(); system != system_managers.end(); ++system)
		(*system)->destroy();
}

