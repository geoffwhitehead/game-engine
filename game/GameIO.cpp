#include "GameIO.h"
#include <Box2D\Box2D.h>
#include "Player.h"
#include "Arena.h"
#include "Bomb.h"
#include "Explosion.h"
#include "NodeHub.h"
#include "Resource.h"


GameIO::GameIO(string root_dir, b2World* b2_world, float ppm) : IOManager(root_dir, b2_world, ppm){

}


GameIO::~GameIO() {
}

void GameIO::loadEntities() {

	for (int i = 0; i < level["entities"].size(); i++) {
		Entity* e;

		if (level["entities"][i]["class"].asString() == "player") {
			class_type = eClassType::CL_PLAYER;
		}
		else if (level["entities"][i]["class"].asString() == "arena") {
			class_type = eClassType::CL_ARENA;
		}
		else if (level["entities"][i]["class"].asString() == "bomb") {
			class_type = eClassType::CL_BOMB;
		}
		else if (level["entities"][i]["class"].asString() == "explosion") {
			class_type = eClassType::CL_EXPLOSION;
		}
		else if (level["entities"][i]["class"].asString() == "node") {
			class_type = eClassType::CL_HUB;
		}
		else if (level["entities"][i]["class"].asString() == "env") {
			class_type = eClassType::CL_ENV;
		}
		else if (level["entities"][i]["class"].asString() == "resource") {
			class_type = eClassType::CL_RESOURCE;
		}

		Json::Value entity = level["entities"][i];

		string name = entity["name"].asString();
		string parent = entity["parent"].asString();
		string group = entity["group"].asString();
		string subgroup = entity["subgroup"].asString();
		Vector3 pos = Vector3(
			entity["position"][0].asFloat(),
			entity["position"][1].asFloat(),
			entity["position"][2].asFloat()
		);
		Mesh* m = findMesh(entity["mesh"].asString());
		Shader* s = findShader(entity["shader"].asString());
		GLuint t = findTexture(entity["texture"].asString());
		bool renderable = entity["is_renderable"].asBool();
		bool physical = entity["is_physical"].asBool();
		bool dynamic = entity["is_dynamic"].asBool();
		bool sensor = entity["is_sensor"].asBool();
		float ppm = pixels_per_m;
		b2World* world = b2_world;
		float friction = entity["friction"].asFloat();
		float density = entity["density"].asFloat();

		// the vars that must e declared outside of switch
		int health;
		int damage;
		int lifetime;
		float c_rad;
		float c_x;
		float c_y;

		switch (class_type) {

		case CL_EXPLOSION:
			lifetime = entity["lifetime"].asFloat();
			c_rad = entity["collision_radius"].asFloat();

			e = new Explosion(name, parent, group, subgroup, pos, m, s, t, renderable, physical, dynamic, sensor, ppm, c_rad, world, friction, density, lifetime, 2);
			break;

		case CL_ARENA:
			c_rad = entity["collision_radius"].asFloat();
			e = new Arena(name, parent, group, subgroup, pos, m, s, t, renderable, physical, dynamic, sensor, ppm, c_rad, world, friction, density);

			break;
		case CL_RESOURCE:
			c_x = entity["collision_x"].asFloat();
			c_y = entity["collision_y"].asFloat();
			e = new Resource(name, parent, group, subgroup, pos, m, s, t, renderable, physical, dynamic, sensor, ppm, c_x, c_y, world, friction, density);

			break;
		case CL_ENV:
			c_x = entity["collision_x"].asFloat();
			c_y = entity["collision_y"].asFloat();
			e = new EnvEntity(name, parent, group, subgroup, pos, m, s, t, renderable, physical, dynamic, sensor, ppm, c_x, c_y, world, friction, density);

			break;
		default:
			cout << "ERROR loading" << endl;
			break;
		}

		in_entity.push_back(e);

	}
}