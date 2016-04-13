#pragma once
#include "../nclgl/OGLRenderer.h"
#include "RenderObject.h"
#include "Entity.h"
#include "Renderer.h"
#include "SystemManager.h"
#include "../engine-io/IOManager.h"

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

private:
	Window window;
	Renderer renderer;
	IOManager* iom;


	vector<SystemManager*> system_managers;

	// these are storage containers that are rarely used, for this reason ive used maps


	vector<Mesh*> meshes;
	vector<std::pair<string, GLuint>> textures;
	vector<Shader*> shaders;
	vector<Entity*> entities;
	vector<IOManager::AudioObject> audio;
};

