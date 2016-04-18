#include "IOManager.h"



IOManager::IOManager(string root_dir, b2World* b2_world, float pixels_per_m) {
	this->root_dir = root_dir;
	this->b2_world = b2_world;
	this->pixels_per_m = pixels_per_m;
}



IOManager::~IOManager() {

}

void IOManager::init() {

}

void IOManager::destroy() {
	// delete textures
	for (int i = 0; i < in_tex.size(); i++) {
		glDeleteTextures(1, &in_tex[i].second);
	}
	// delete meshes
	for (int i = 0; i < in_mesh.size(); i++) {
		delete in_mesh[i].second;
	}
	// delete shaders
	for (int i = 0; i < in_shader.size(); i++) {
		delete in_shader[i].second;
	}
	// delete entities
	for (int i = 0; i < in_entity.size(); i++) {
		delete in_entity[i];
	}

}

void IOManager::loadTextures() {
	for (int i = 0; i < level["textures"].size(); i++) {
		string name = level["textures"][i]["name"].asString();
		string path = level["textures"][i]["path"].asCString();
		in_tex.push_back(pair<string, GLuint>(name, LoadTexture(path.c_str(), true)));
	}
}

void IOManager::loadMeshes() {

	for (int i = 0; i < level["meshes"].size(); i++) {
		Mesh* mesh;
		
		string function = level["meshes"][i]["function"].asString();
		if (function == "GenerateTriFan") {
			float x = level["meshes"][i]["x"].asFloat();
			float y = level["meshes"][i]["y"].asFloat();
			float z = level["meshes"][i]["z"].asFloat();
			float w = level["meshes"][i]["w"].asFloat();
			float radius = level["meshes"][i]["radius"].asFloat();
			mesh = Mesh::GenerateTriFan(radius, Vector4(x, y, z, w));
		}
		else if (function == "GenerateQuad") {
			float x = level["meshes"][i]["x"].asFloat();
			float y = level["meshes"][i]["y"].asFloat();
			mesh = Mesh::GenerateQuad(x, y);
		}
		in_mesh.push_back(pair<string, Mesh*>(level["meshes"][i]["name"].asString(), mesh));
	}
}

void IOManager::loadShaders() {
	for (int i = 0; i < level["shaders"].size(); i++) {
		Shader* shader;

		string name = level["shaders"][i]["name"].asString();
		string vert = level["shaders"][i]["vert"].asString();
		string frag = level["shaders"][i]["frag"].asString();

		in_shader.push_back(pair<string, Shader*>(name, new Shader(vert, frag)));
	}
}

void IOManager::loadAudio() {
	for (int i = 0; i < level["audio"].size(); i++) {

		AudioObject* audio = new AudioObject;
		audio->name = level["audio"][i]["name"].asString();
		audio->path = level["audio"][i]["path"].asCString();
		audio->default_volume = level["audio"][i]["default_volume"].asFloat();
		in_audio.push_back(audio);
	}
}

void IOManager::load(string file_name) {
	//clearLevel(); TODO: impleent this
	level_path = file_name;
	setLevel(file_name);
	loadAudio();
	loadMeshes();
	loadShaders();
	loadTextures();
	loadEntities(); // load entities last as it requires the others
}

// read in all objects from file "path_to_level"
void IOManager::setLevel(std::string path_to_level) {
	std::string data = root_dir + path_to_level;
	Json::Reader reader;
	ifstream fs(data);
	if (!fs) cout << "Failed to read from path.";
	if (!reader.parse(fs, level)) cout << "Failed to parse.";

}

// clears all the objects ready for new objects to be loaded from file
void IOManager::clearLevel() {
	
	// delete textures
	for (int i = 0; i < in_tex.size(); i++) {
		glDeleteTextures(1, &in_tex[i].second);
	}
	// delete meshes
	for (int i = 0; i < in_mesh.size(); i++) {
		delete in_mesh[i].second;
	}
	// delete shaders
	for (int i = 0; i < in_shader.size(); i++) {
		delete in_shader[i].second;
	}
	// delete entities
	for (int i = 0; i < in_entity.size(); i++) {
		delete in_entity[i];
	}

	
	// empty the vectors
	in_audio.clear();
	in_entity.clear();
	in_mesh.clear();
	in_shader.clear();
	in_tex.clear();
}

GLuint IOManager::LoadTexture(const char* filename, bool textureRepeating = true) {
	GLuint texture = SOIL_load_OGL_texture(filename,
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_MULTIPLY_ALPHA
	);

	if (texture == NULL) {
		printf("[Texture loader] \"%s\" failed to load!\n", filename);
		return 0;
	}
	else {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureRepeating ? GL_REPEAT : GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureRepeating ? GL_REPEAT : GL_CLAMP);

		glActiveTexture(0);
		return texture;
	}
}

GLuint IOManager::findTexture(string name) {
	if (name != "") {
		for (int i = 0; i < in_tex.size(); i++) {
			if (in_tex[i].first == name) {
 				return in_tex[i].second;
			}
		}
		cout << "a texture of this name wasnt found - exiting" << endl;
		exit(1);
	}
}

Mesh* IOManager::findMesh(string name) {
	if (name != "") {
		for (int i = 0; i < in_mesh.size(); i++) {
			if (in_mesh[i].first == name) {
				return in_mesh[i].second;
			}
		}
		cout << "a mesh of this name wasnt found - exiting" << endl;
		exit(1);
	}
}

Shader* IOManager::findShader(string name) {
	if (name != "") {
		for (int i = 0; i < in_shader.size(); i++) {
			if (in_shader[i].first == name) {
				return in_shader[i].second;
			}
		}
		cout << "a shader of this name wasnt found - exiting" << endl;
		exit(1);
	}
}