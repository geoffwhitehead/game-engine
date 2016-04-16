#include "GameIO.h"
#include <Box2D\Box2D.h>
#include "Player.h"



GameIO::GameIO(string root_dir, b2World* b2_world, float ppm) : IOManager(root_dir, b2_world, ppm){

}


GameIO::~GameIO() {
}

void GameIO::loadEntities() {

	
	for (int i = 0; i < level["entities"].size(); i++) {
		string name = level["entities"][i]["name"].asString();
		string parent = level["entities"][i]["parent"].asString();
		string group = level["entities"][i]["group"].asString();
		string subgroup = level["entities"][i]["subgroup"].asString();
		Vector3 pos = Vector3(
			level["entities"][i]["position"][0].asFloat(),
			level["entities"][i]["position"][1].asFloat(),
			level["entities"][i]["position"][2].asFloat()
		);
		Mesh* m = findMesh(level["entities"][i]["mesh"].asString());
		Shader* s = findShader(level["entities"][i]["shader"].asString());
		GLuint t = findTexture(level["entities"][i]["texture"].asString());
		bool is_renderable = level["entities"][i]["is_renderable"].asBool();
		bool is_physical = level["entities"][i]["is_physical"].asBool();
		float radius = level["entities"][i]["collision_radius"].asFloat();
		float density = level["entities"][i]["density"].asFloat();
		float friction = level["entities"][i]["friction"].asFloat();

		if (level["entities"][i]["class"].asString() == "player") {
			in_entity.push_back(
				new Player(name, parent, group, subgroup, pos, m, s, t, is_renderable, is_physical, pixels_per_m, radius, b2_world, friction, density)
			);
		
		}

	}
}