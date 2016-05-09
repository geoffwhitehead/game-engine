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

void GameManager::removeEntity(Entity* e) {
	auto it = std::find(entities.begin(), entities.end(), e);
	if (it != entities.end()) { entities.erase(it); }
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

// check to make sure no duplicate deletions are entered into the list
void GameManager::markToDelete(Entity* e) {
	cout << "deleting comsthing " << endl;
	for (int i = 0; i < marked_for_deletion.size(); i++) {
		if (e == marked_for_deletion[i]) {
			return;
		}
	}
	marked_for_deletion.push_back(e);
}


// main game loop
void GameManager::run(){
	
	// initialise systems
	for (vector<SystemManager*>::iterator system = system_managers.begin(); system != system_managers.end(); ++system)
		(*system)->init();

	while (window.UpdateWindow()){
		float msec = window.GetTimer()->GetTimedMS();

		msec *= 2.0f;

		//DrawText("This is orthographic text!", Vector3(0, 0, 0), 16.0f);

		// update systems
		for (vector<SystemManager*>::iterator system = system_managers.begin(); system != system_managers.end(); ++system) {
			(*system)->update(msec);
		}

		// update entities
		for (vector<Entity*>::iterator entity = entities.begin(); entity != entities.end(); ++entity) {
			(*entity)->update(msec);
		}

		// mark entities to remove
		for (vector<Entity*>::iterator entity = marked_for_deletion.begin(); entity != marked_for_deletion.end(); ++entity) {
			(*entity)->delete_me = true;
		}

		// remove all references from entity list
		for (int i = 0; i < marked_for_deletion.size(); i++){
			entities.erase(std::remove(entities.begin(), entities.end(), marked_for_deletion[i]), entities.end());
		}
		

		// delete entities in marked for deletion list
		for (vector<Entity*>::iterator entity = marked_for_deletion.begin(); entity != marked_for_deletion.end(); ++entity) {
			delete((*entity));
		}
		
		// clear marked list
		marked_for_deletion.clear();

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


/*
void GameManager::DrawText(const std::string &text, const Vector3 &position, const float size) {
	//Create a new temporary TextMesh, using our line of text and our font
	TextMesh* mesh = new TextMesh(text, *basicFont);

	//This just does simple matrix setup to render in either perspective or
	//orthographic mode, there's nothing here that's particularly tricky.
	
	//In ortho mode, we subtract the y from the height, so that a height of 0
	//is at the top left of the screen, which is more intuitive
	//(for me anyway...)
	//modelMatrix = Matrix4::Translation(Vector3(position.x, height - position.y, position.z)) * Matrix4::Scale(Vector3(size, size, 1));
	//viewMatrix.ToIdentity();
	//projMatrix = Matrix4::Orthographic(-1.0f, 1.0f, (float)width, 0.0f, (float)height, 0.0f);

	//Either way, we update the matrices, and draw the mesh
	//renderer.UpdateShaderMatrices();
	mesh->Draw();

	delete mesh; //Once it's drawn, we don't need it anymore!
}

*/