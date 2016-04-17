#pragma once

#include <string>
#include <vector>
#include "../_resources/jsoncpp/include/json/json.h"
#include "../engine-base/Entity.h"


using namespace std;

class IOManager {
	
	
public:
	struct AudioObject {
		string name;
		string path;
		float default_volume;
	};
	b2World* b2_world;
	float pixels_per_m;



	IOManager(string root_dir, b2World* b2_world, float pixels_per_m);
	virtual ~IOManager();

	

	void load(string file_name);
	GLuint LoadTexture(const char* filename, bool textureRepeating);

	void init();
	void destroy();

	GLuint findTexture(string name);
	Mesh* findMesh(string name);
	Shader* findShader(string name);

	vector<pair<string, GLuint>> in_tex;
	vector<pair<string, Mesh*>> in_mesh;
	vector<pair<string, Shader*>> in_shader;
	vector<Entity*> in_entity;
	vector<AudioObject*> in_audio;
	
	string level_path; // this is the path to the level from the root
	string root_dir; // this is the path to the root directory containing the levels
	Json::Value level;   // 'level' will contain the root value after parsing.

	virtual void loadEntities() = 0;
	void loadTextures();
	void loadMeshes();
	void loadShaders();
	void loadAudio();
	
	void setLevel(std::string file_name);
	void clearLevel();
};

