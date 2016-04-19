#pragma once
#include "../engine-base/SystemManager.h"
#include "../engine-base/SubSystem.h"
#include "../engine-io/IOManager.h"
#include "../_resources/irrKlang-1.5.0/include/irrKlang.h"
#include <iostream>
#include <vector>
#include <map>

using namespace irrklang;
using namespace std;


/*
*	Class: AudioManager
*	Author:	Geoff Whitehead
*	Description: Handles all irrKlang audio related things. This includes loading the sound engine and 
*		exposing functuion that enable the user to load sounds. Has a list of sub systems that the user
*		can add to and will call update on these each game loop. 
*/

class AudioManager :
	public SystemManager
{
public:
	AudioManager();
	AudioManager(IOManager* iom);
	~AudioManager();
	void update(float);
	void destroy();
	void init();
	void addSubSystem(SubSystem*);
	void clear();

	ISoundSource* loadSound(string name, const ik_c8* path, float default_volume = 1.0f);
	ISoundSource* getSoundSource(string name);
	void play2D(string name);
	
	ISoundEngine* getSoundEngine();
private:
	ISoundEngine* se;
	map<string, ISoundSource*> audio;
	IOManager* iom;
	void loudAudioFromFile();
	void createSoundDevice();
	vector<SubSystem*> sub_systems;
};

