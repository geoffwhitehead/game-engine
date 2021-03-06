#pragma once
#include "../engine-base/GameManager.h"
#include "../engine-base/SubSystem.h"
#include "../engine-base/GameLogicManager.h"
#include "Player.h"
#include "Bomb.h"
#include "Arena.h"
#include "Explosion.h"
#include "Shield.h"
#include "Node.h"
#include "LevelEntity.h"
#include "ConnectionManager.h"
#include "Connector.h"
#include "NodeHub.h"
#include "NodeHubResource.h"
#include "NodeHubShield.h"
#include "../engine-audio/AudioManager.h"
#include "Explosion.h"
#include <math.h>
#include <chrono>
#include <ctime>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <stdlib.h>   


/*
*	Class: GameLogic
*	Author:	Geoff Whitehead
*	Description: Handles all game evntrs and applies game logic to them. Handles states and 
*		functions that occur because of states or events.
*/

using namespace std::chrono;
class GameLogic :
	public SubSystem
{
public:

	enum ePlayerTurn { GS_PLAYER_1, GS_PLAYER_2 };

	enum eGameState { GS_PLAYING, GS_FIRING, GS_BUILDING, GS_EXPLODING, GS_CHARGING, 
		GS_CONTACT, GS_CAMERA_MOVING, GS_GAME_END, GS_FIRING_CLUSTERS, GS_COLLIDING, 
		GS_ACTIVATING_CONNECTORS, GS_DESTROYING_CONNECTORS, GS_SURGE
	};
	enum eInputEvents { IE_LEFT_CLICK, IE_ENTER, IE_SPACE, IE_KEY_TAB, IE_LEFT, 
		IE_RIGHT, IE_PAD1, IE_PAD2, IE_PAD3, IE_PAD4, IE_PAD5, IE_PAD6, IE_KEY_A, IE_KEY_D, IE_KEY_ADD
	};
	enum eAudioEvents { AE_TURN_SWAP, AE_EXPLOSION_BOMB, AE_MOVE, AE_POWERUP, 
		AE_HUB_DAMAGED, AE_HUB_DESTROYED, AE_INSUF_RESOURCE, AE_POWERUP_RESOURCE,
		AE_POWERDOWN_RESOURCE, AE_CHARGE_1, AE_CHARGE_2, AE_CHARGE_FULL, AE_LAUNCH,
		AE_SELECT, AE_SHIELD_ON, AE_SHIELD_OFF, AE_CONNECTOR_PLACED, AE_CONNECTOR_DOWN,
		AE_SURGE_EXPLOSION, AE_SHIELD_COLLISION, AE_SURGE_EXPLOSION_INIT, AE_REPAIR, AE_START, AE_END
	};
	enum eGameEvents { GE_QUIT, GE_NODE_DESTROYED };

	enum eActionSelection {AS_HUB, AS_RESOURCE_HUB, AS_BOMB, AS_CLUSTER, AS_SHIELD_HUB, 
		AS_SURGE_BOMB, AS_REPAIR
	};

	enum eConDirection {CD_LEFT, CD_RIGHT};

	//game vars
	const float ROTATION = 0.002f;
	const float MAX_CHARGE = 20.0f;
	const int MAX = 20;
	const float pointer_offset = 12.0f;
	const float charge_speed = 0.020f;
	const float initial_charge = 0.0125f;
	const int starting_resource = 11;
	const float ppm = 30;
	const float node_damage = 1;
	const int kill = 50;
	const float board_depth = -7.5;
	const float damping = 0.00033f;
	const float damping_cluster = 0.00044;
	const Vector3 vec0 = Vector3(0.0, 0.0, 0.0);
	const Vector3 VEC_STILL = Vector3(0.0, 0.0, 5.0);

	const Vector3 p1_start_loc = Vector3(-80, -70, 0);
	const Vector3 p2_start_loc = Vector3(80, 70, 0);
	Player* p1;
	Player* p2;
	eGameState game_state;
	ePlayerTurn player_turn;
	Player* active_player;
	eActionSelection action = AS_HUB;
	Entity* fired_entity;
	vector<Explosion*> explosions;
	const int c_steps = 5; // used to cause a lag for when to check connector collisions. Avoids different collision events occuring at the same time.
	int c_step;
	const int wait = 80; // frames to wait between connections destroyed
	const int surge_lag = 12;
	
	enum eFilter {
		eNoCollide = 0x000,
		eNode = 0x0001,
		eProjectile = 0x0002,
		eExplosion = 0x0004,
		eShield = 0x0008,
		eBlock = 0x0010,
		eResource = 0x0011,
		ePointer = 0x0012,
		eConnector = 0x0014,
		eDisablers = 0x0016
	};


	// camera stuff
	const int camera_steps = 15;
	int step_counter = camera_steps;
	Vector2 target_pos;
	Vector2 direction_step;

	// cluster camera stuff
	int cluster_counter;
	int cluster_index;
	bool all_clusters_fired;
	int clusters_fired = 0;
	Entity* cluster_ptr_to_follow;
	const int cluster_to_follow = 1;

	// charge stuff
	const float clamp = 0.033f;
	const int charge_arr_size = 5;
	bool fully_charged;
	const float charge_step = ((clamp) / 5);
	const float charge_arr[5] = { initial_charge +(charge_step), initial_charge + (charge_step*2), initial_charge + (charge_step*3), initial_charge + (charge_step*4), initial_charge + (charge_step*5) };
	int current_charge_index = 0;
	bool charging;
	time_point<system_clock> start;
	time_point<std::chrono::system_clock> end;
	duration<double> elapsed_seconds;

	// mesh selection stuff
	const string node_hub = "hub";
	const string mesh_surge_bomb = "mesh_bomb_surge";
	const string node_resource_hub = "resource_hub";
	const string node_shield_hub = "shield_hub";
	const string p1_mesh = "p1_mesh_";
	const string p2_mesh = "p2_mesh_";
	const string mesh_connector = "connector";
	const string mesh_ex_surge = "mesh_ex_surge";
	const string mesh_repair = "mesh_repair";
	const string mesh_ex_repair = "mesh_ex_repair";

	const int explosion_z = 10;
	const int bomb_z = 8;
	const int shield_z = 0;
	const int hub_z = 4;
	const int camera_z = 20;
	const int pointer_z = 14;
	const int connector_z = 2;


	//subgroup
	const string subgroup_hub_shield = "shield_hub";
	const string subgroup_hub_resource = "resource_hub";
	const string subgroup_hub = "";
	const string subgroup_bomb = "bomb";
	const string subgroup_surge = "surge";
	const string subgroup_surge_initial = "surge_initial";
	const string subgroup_repair = "repair";

	// groups
	const string group_bomb = "bombs";
	const string group_surge_bomb = "surge";
	const string group_env_resource = "resource";
	const string group_env_block = "impas";
	const string group_explosion = "explosions";
	const string group_shield = "shields";
	const string group_hub = "hubs";
	const string group_connector = "connectors";
	const string group_repair = "repair";

	// explosion energy
	const string mesh_ex_energy = "mesh_explosion_10";
	const float ex_energy_radius = 10.0;
	const float ex_energy_damage = 5.0;
	const float ex_energy_lifetime = 30;

	//bomb
	const float bomb_radius = 1.0;
	const string mesh_ex_bomb = "mesh_explosion_2";
	const int bomb_cost = 1;

	// bomb explosion
	const float ex_bomb_radius = 2.0;
	const int ex_bomb_lifetime = 30;
	const float ex_bomb_damage = 3.0;

	// resource hub
	const int resource_hub_strength = 3;
	const int resource_hub_health = 5;
	const int resource_hub_cost = 7;
	const float resource_hub_radius = 2.0;

	// connectors
	const float con_spread = 2.5;
	const float con_placement_min = 4.0;
	float con_last_placement = 0.0;
	const float con_radius = 1.0;
	const float con_health = 1.0;
	const float con_cost = 0.0;
	Connector* last_con_ptr = NULL;
	const float min_con_velocity = 0.0110;
	Connector* activating_connector;
	Connector* destroy_left;
	Connector* destroy_right;
	bool collision_flag;
	bool hub_connector_collision_flag;
	bool con_con_collision_flag = false;
	bool activation_collision_flag;

	// node hub
	const int hub_radius = 2.0;
	const int hub_cost = 7;
	const int hub_health = 5;

	// shield hub
	const int shield_hub_health = 5;
	const float shield_hub_radius = 2.0;
	const float shield_radius = 15.0;

	// repair
	const int repair_cost = 1;
	const int repair_amount = 1;
	const float repair_radius = 1.0;
	const float ex_repair_radius = 1.5;
	const int ex_repair_lifetime = 40;
	bool repair_flag;

	// shield
	#define mesh_shield "mesh_shield"
	const int shield_power = 1;
	const int shield_hub_cost = 7;
	bool shield_collision_flag;

	// cluster bomb
	const int cluster_firing_time = 20;
	const int cluster_count = 3;
	const int cluster_timing_arr[4] = { 20, 10, 0 }; // must be in descending order
	const string mesh_bomb_cluster = "mesh_bomb_cluster";
	const float bomb_cluster_radius = 1.5;
	const int bomb_cluster_cost = 1;
	const float cluster_spread = 0.12;
	vector<Bomb*> vec_clusters;
	
	// cluster explosion
	const string mesh_explosion_cluster = "mesh_explosion_2";
	const float explosion_cluster_radius = 2.0;
	const int explosion_cluster_lifetime = 20;
	const int explosion_cluster_damage = 1.5;

	//surge bomb
	const float surge_bomb_radius = 1;
	const int surge_cost = 3;
	const int surge_max_damage = 3;
	const int surge_min_damage = 1;
	const float ex_surge_radius = 1.5;
	const int ex_surge_lifetime = 20;
	bool surge_flag = false;
	float current_surge_damage;
	float surge_deterioration_speed = 0.1;
	float surge_left_damage;
	float surge_right_damage;
	Connector* surge_left;
	Connector* surge_right;
	Node* left_node_surge;
	Node* right_node_surge;
	bool right_surged;
	bool left_surged;
	bool surge_priority_found;
	Connector* first_surge_contact;


	GameLogic(GameManager* gm, GameLogicManager* glm, b2World* world, AudioManager* am, Camera* cam);
	~GameLogic();

	void init();
	void update(float msec);
	static b2Vec2 force;
	void removeEdges(Node*n);
	void GameLogic::getConnectorsToRemove(vector<Entity*>* results, Connector*c, enum eConDirection dir);
	void destroyConnector(Connector*c);
	void connectorIntersection(Connector* c1, Connector* c2);
	void destroyUnconnectedNodes();
	void removeEdge(vector<Entity*>* path);

	void handleEvents();
	void handleStates();
	void endTurn();
	void setPointer();
	bool sufficientResource();
	void checkResourceHubs();
	void showInstructions();
	void applyRepair(Node*n, float repair_amt);

	void createExplosion(Vector3 pos, string subgroup, string mesh, float ex_radius, int lifetime, int damage);
	void destroyNode(Node* n);
	b2Vec2 getTrajectory(Entity* origin);
	void fireWeapon(Entity* e);
	void launchNode(Entity* e);
	void launch();
	void destroy();
	void handleCollisions();
	bool isAwake(Entity*);
	void adjustDirection(eInputEvents);
	void applyDamage(Node*, float);
	void applyDamage(Shield* shield, float damage);
	void powerupShields(Player* p);
	Connector* placeConnector(Vector3 pos, string mesh, float con_radius, float health, float cost);
	void surge(Connector*c);
	Vector3 constructVector(Vector3 pos, float z);

	void moveCamera(Node* n);
	void applyResource(NodeHubResource* n);
	void detachResource(NodeHubResource* n);
	bool exists(vector<LevelEntity*>* vector, LevelEntity* to_find);
	bool shield_hub_placed = false;
	void setFixture(Entity*, uint16 category_bits, uint16 mask_bits);
	Node* findNextNode();
	Node* findPreviousNode();
	string getName();

	void editEntity(string name, string parent, bool is_collidable, bool is_renderable);
	Vector3 getMousePos3D();
	

	b2World* world;

	vector<eInputEvents> in_input_events;
	vector<eAudioEvents> out_audio_events;
	vector<pair<Entity*, Entity*>> in_contact_events;
	vector<eGameEvents> in_game_events;
private:
	GameLogicManager* glm;
	GameManager* gm;
	AudioManager* am;
	Camera* cam;
	Entity* pointer;
	Entity* pointer_mark;
};

