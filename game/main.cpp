#include "../nclgl/OGLRenderer.h"

#include "../engine-base/Camera.h"
#include "../engine-base/GameManager.h"
#include "../engine-physics/CollisionManager.h"
#include "../engine-audio/AudioManager.h"
#include "../engine-input/InputManager.h"
#include "../engine-base/EventManager.h"
#include "../engine-base/GameLogicManager.h"

#include "GameInput.h"
#include "GameAudio.h"
#include "CollisionResponse.h"
#include "GameEvents.h"
#include "GameLogic.h"

#include <iostream>
#include <map>
#include "../_resources/jsoncpp/dist/json/json.h"



#define W_X 1024.0f
#define W_Y 768.0f

std::map<std::string, Mesh*> map_meshes;
std::map<const std::string, GLuint> map_textures;
std::map<std::string, Entity*> map_entities;
std::map<std::string, Shader*> map_shaders;

void main(void) {

	std::string data = "./levels/data.json";
	Json::Value root;   // 'root' will contain the root value after parsing.
	Json::Reader reader;
	ifstream fs(data);
	if (!fs) cout << "Failed to read from path.";
	if (!reader.parse(fs, root)) cout << "Failed to parse.";

	// GAME MANAGER
	GameManager *gm = new GameManager(W_X, W_Y);

	//ADD SYSTEM MANAGERS
	CollisionManager* cm = new CollisionManager();
	AudioManager* am = new AudioManager();
	InputManager* im = new InputManager();
	EventManager* em = new EventManager();
	GameLogicManager* glm = new GameLogicManager();

	//CREATE SUB SYSTEMS
	GameEvents* ge = new GameEvents(gm);

	Camera* camera = new Camera(0.0f, 0.0f, Vector3(0, 0, 400), W_X, W_Y);
	Camera::projMatrix = Matrix4::Orthographic(1, 1000, W_X/4.0f, -W_X/4.0f, W_Y/4.0f, -W_Y/4.0f);
	//Camera::viewMatrix = Matrix4::BuildCamera(Vector3(0.0, 50.0, 20.0), Vector3(0.0, 0.0, 0.0));
		
		//camera->BuildViewMatrix();
	
	GameInput* gi = new GameInput(gm, ge, camera);
	GameAudio* ga = new GameAudio(am, ge);
	CollisionResponse* cr = new CollisionResponse(cm, ge);
	GameLogic* gl = new GameLogic(glm, ge);
	
	// JSON STUFF
	Json::Value level = root["level"][0];


	//TEXTURES
	for (int i = 0; i < level["textures"].size(); i++)
	{
		string name = level["textures"][i]["name"].asString();
		GLuint tex = gm->LoadTexture(level["textures"][i]["path"].asCString());
		map_textures.insert(pair <string, GLuint>(name, tex));
	}

	//AUDIO
	for (int i = 0; i < level["audio"].size(); i++)
	{
		string name = level["audio"][i]["name"].asString();
		string path = level["audio"][i]["path"].asCString();
		float vol = level["audio"][i]["default_volume"].asFloat();
		am->loadSound(name, path.c_str(), vol);
	}

	//SHADERS
	for (int i = 0; i < level["shaders"].size(); i++)
	{
		Shader* s = new Shader(level["shaders"][i]["vert"].asString(), level["shaders"][i]["frag"].asString());
		map_shaders.insert(pair <string, Shader*>(level["shaders"][i]["name"].asString(), s));
	}

	//MESHES
	for (int i = 0; i < level["meshes"].size(); i++)
	{
		string function = level["meshes"][i]["function"].asString();
		Mesh * m;
		if (function == "GenerateQuad") {
			m = Mesh::GenerateQuad(level["meshes"][i]["x"].asFloat(), level["meshes"][i]["y"].asFloat());
		}
		else if (function == "GenerateTriFan") {
			m = Mesh::GenerateTriFan(level["meshes"][i]["radius"].asFloat(), Vector4(level["meshes"][i]["x"].asFloat(), level["meshes"][i]["y"].asFloat(), level["meshes"][i]["z"].asFloat(), level["meshes"][i]["w"].asFloat()));
		}
		map_meshes.insert(pair<string, Mesh*>(level["meshes"][i]["name"].asString(), m));
	}

	//ENTITIES
	for (int i = 0; i < level["entities"].size(); i++)
	{
		Entity *e = new Entity(
			level["entities"][i]["name"].asString(),
			level["entities"][i]["group"].asString(),
			level["entities"][i]["subgroup"].asString(),
			Vector3(
				level["entities"][i]["position"][0].asFloat(),
				level["entities"][i]["position"][1].asFloat(),
				level["entities"][i]["position"][2].asFloat()
				),
			Vector3(
				level["entities"][i]["acceleration"][0].asFloat(),
				level["entities"][i]["acceleration"][1].asFloat(),
				level["entities"][i]["acceleration"][2].asFloat()
				),
			Vector3(
				level["entities"][i]["velocity"][0].asFloat(),
				level["entities"][i]["velocity"][1].asFloat(),
				level["entities"][i]["velocity"][2].asFloat()
				),
			map_meshes[level["entities"][i]["mesh"].asString()],
			map_shaders[level["entities"][i]["shader"].asString()],
			map_textures[level["entities"][i]["texture"].asString()]
			);

		e->setMass(level["entities"][i]["mass"].asFloat());

		if (level["entities"][i]["hidden"].asBool() == true) {
			e->is_renderable = false;
		}
			
		if (level["entities"][i]["collidable"].asBool()) {

			if (level["entities"][i]["collision_object"].asString() == "CIRCLE")
				cm->addObject(e, level["entities"][i]["collision_radius"].asFloat());
			if (level["entities"][i]["collision_object"].asString() == "PLANE")
				cm->addObject(e, e->getPhysicsObject()->getPos().Length(), 
					Vector3(
						level["entities"][i]["normal"][0].asFloat(),
						level["entities"][i]["normal"][1].asFloat(),
						level["entities"][i]["normal"][2].asFloat()
						));
		}
		if (level["entities"][i]["parent"].asString() == "")
			map_entities.insert(pair<string, Entity*>(level["entities"][i]["name"].asString(), e));
		else
			map_entities[level["entities"][i]["parent"].asString()]->addChild(e); 
	}

	// register entities
	for (auto iterator = map_entities.begin(); iterator != map_entities.end(); iterator++)
		gm->addEntity(iterator->second);

	//register managers
	gm->addSystemManager(am);
	gm->addSystemManager(cm);
	gm->addSystemManager(im);
	gm->addSystemManager(em);
	gm->addSystemManager(glm);

	//register sub systems
	im->addSubSystem(camera);
	glm->addSubSystem(gl);
	im->addSubSystem(gi);
	am->addSubSystem(ga);
	cm->addSubSystem(cr);
	em->addSubSystem(ge);
	
	gm->run();

}