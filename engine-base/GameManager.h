#pragma once

#include "../engine-base/Entity.h"
#include "../engine-base/SystemManager.h"
#include "../nclgl/Window.h"
#include <vector>
#include "../engine-io/IOManager.h"
#include "Renderer.h"

/*
*	Class: GameManager
*	Author:	Geoff Whitehead
*	Description: Handles all game related things. Holds a list of all the sytem managers to update. 
		Initiates shutdown. Central class for all other class to access to gain game information such 
		as list of entities
*/

using namespace std;


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
	IOManager* iom;
	void initTextRendering();
	void RenderText(Shader &s, std::string text, GLfloat x, GLfloat y, GLfloat scale, Vector3 color);

	



private:
	Window window;
	Renderer renderer;

	struct Character {
		GLuint TextureID;  // ID handle of the glyph texture
		Vector2	Size;       // Size of glyph
		Vector2	Bearing;    // Offset from baseline to left/top of glyph
		GLuint Advance;    // Offset to advance to next glyph
	};
	GLuint VAO, VBO;
	std::map<GLchar, Character> Characters;
	b2World* world;
	vector<SystemManager*> system_managers;

};

