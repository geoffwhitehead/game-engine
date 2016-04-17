#include "GameIO.h"
#include <Box2D\Box2D.h>
#include "Player.h"
#include "Arena.h"
#include "Bomb.h"
#include "Explosion.h"

enum eClassType {CL_PLAYER, CL_ARENA, CL_BOMB, CL_EXPLOSION};
eClassType class_type;

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
		float c_rad = entity["collision_radius"].asFloat();
		b2World* world = b2_world;
		float friction = entity["friction"].asFloat();
		float density = entity["density"].asFloat();


		// ther vars that must e declared outside of switch
		int health;
		int damage;
		int lifetime;


		switch (class_type) {
		case CL_PLAYER:

			health = entity["health"].asFloat();

			e = new Player(name, parent, group, subgroup, pos, m, s, t, renderable, physical, dynamic, sensor, ppm, c_rad, world, friction, density, health);
				
			break;

		case CL_BOMB:
			damage = entity["damage"].asFloat();

			e = new Bomb(name, parent, group, subgroup, pos, m, s, t, renderable, physical, dynamic, sensor, ppm, c_rad, world, friction, density, damage);
			break;

		case CL_EXPLOSION:
			lifetime = entity["lifetime"].asFloat();

			e = new Explosion(name, parent, group, subgroup, pos, m, s, t, renderable, physical, dynamic, sensor, ppm, c_rad, world, friction, density, lifetime);
			break;

		case CL_ARENA:
			e = new Arena(name, parent, group, subgroup, pos, m, s, t, renderable, physical, dynamic, sensor, ppm, c_rad, world, friction, density);

			break;

			
		default:
			cout << "error loading" << endl;
 			exit(1);
			break;
		}

		in_entity.push_back(e);

	}
}