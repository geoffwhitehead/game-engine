#pragma once

#include "../engine-base/Entity.h"
#include "../engine-base/SystemManager.h"
#include "../nclgl/Window.h"
#include <vector>
#include "../engine-io/IOManager.h"
#include "Renderer.h"


using namespace std;

// init gm with reference to input handler, ui, camera, audio, memory manager 

class GameManager
{
public:
	GameManager(float, float);
	~GameManager();

	void run();
	void addEntity(Entity*);
	void addSystemManager(SystemManager*);
	Window* getWindow();
	vector<Entity*>* getEntities();
	Entity* getEntityByName(string child_name, string parent_name = "");
	void addFileInput(IOManager* iom);
	void loadLevel(string file_name);
	void ShutDown();
	void clear();
	vector<Entity*> entities;

private:
	Window window;
	Renderer renderer;
	IOManager* iom;
	b2World* world;
	vector<SystemManager*> system_managers;

};

