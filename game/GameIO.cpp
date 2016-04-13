#include "GameIO.h"



GameIO::GameIO(CollisionManager* cm, string root_dir) : IOManager(root_dir){
	this->cm = cm;
}


GameIO::~GameIO() {
}

void GameIO::loadEntities() {

	for (int i = 0; i < level["entities"].size(); i++) {
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
			IOManager::findMesh(level["entities"][i]["mesh"].asString()),
			IOManager::findShader(level["entities"][i]["shader"].asString()),
			IOManager::findTexture(level["entities"][i]["texture"].asString())
		);

		e->setMass(level["entities"][i]["mass"].asFloat());

		if (level["entities"][i]["hidden"].asBool() == true) {
			e->is_renderable = false;
		}

		if (level["entities"][i]["collidable"].asBool()) {
			/*
			if (level["entities"][i]["collision_object"].asString() == "CIRCLE")
				cm->addObject(e, level["entities"][i]["collision_radius"].asFloat());
			if (level["entities"][i]["collision_object"].asString() == "PLANE")
				cm->addObject(e, e->getPhysicsObject()->getPos().Length(),
					Vector3(
						level["entities"][i]["normal"][0].asFloat(),
						level["entities"][i]["normal"][1].asFloat(),
						level["entities"][i]["normal"][2].asFloat()
					));
		*/
		}
		/*
		if (level["entities"][i]["parent"].asString() == "")
			map_entities.insert(pair<string, Entity*>(level["entities"][i]["name"].asString(), e));
		else
			map_entities[level["entities"][i]["parent"].asString()]->addChild(e);
		*/
	}
}