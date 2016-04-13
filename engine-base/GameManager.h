#pragma once
#include "../nclgl/OGLRenderer.h"
#include "RenderObject.h"
#include "Entity.h"
#include "Renderer.h"
#include "SystemManager.h"

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

	GLuint LoadTexture(const char* filename, bool textureRepeating = true);

private:
	Window window;
	Renderer renderer;
	vector<Entity*> entities;
	vector<GLuint> textures;
	vector<SystemManager*> system_managers;

	
};

