#include "GameManager.h"
#include <windows.h>
#include <ft2build.h>
#include FT_FREETYPE_H  

GameManager::GameManager(float w_x, float w_y)
: window(Window(w_x, w_y)), renderer(Renderer(window)){

	

}

void GameManager::initTextRendering() {
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	FT_Face face;
	if (FT_New_Face(ft, "fonts/arial.ttf", 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

	FT_Set_Pixel_Sizes(face, 0, 48);

	if (FT_Load_Char(face, 'X', FT_LOAD_RENDER))
		std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

	for (GLubyte c = 0; c < 128; c++) {
		// Load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Now store character for later use
		Character character = {
			texture,
			Vector2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			Vector2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(std::pair<GLchar, Character>(c, character));
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);



}



void GameManager::RenderText(Shader &s, std::string text, GLfloat x, GLfloat y, GLfloat scale, Vector3 color) {
	// Activate corresponding render state	
	//s.Use();
	glUniform3f(glGetUniformLocation(s.GetShaderProgram(), "textColor"), color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);

	// Iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++) {
		Character ch = Characters[*c];

		GLfloat xpos = x + ch.Bearing.x * scale;
		GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;
		// Update VBO for each character
		GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 1.0 },

			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 }
		};
		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
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

		RenderText(*(iom->findShader("font")), "This is sample text", 25.0f, 25.0f, 1.0f, Vector3(0.5, 0.8f, 0.2f));
		RenderText(*(iom->findShader("font")), "(C) LearnOpenGL.com", 540.0f, 570.0f, 0.5f, Vector3(0.3, 0.7f, 0.9f));

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

