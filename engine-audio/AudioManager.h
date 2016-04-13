#pragma once
#include "../engine-base/SystemManager.h"
#include "../engine-base/SubSystem.h"
#include "../_resources/irrKlang-1.5.0/include/irrKlang.h"
#include <iostream>
#include <vector>
#include <map>

using namespace irrklang;
using namespace std;


class AudioManager :
	public SystemManager
{
public:
	AudioManager();
	~AudioManager();
	void update(float);
	void destroy();
	void init();
	void addSubSystem(SubSystem*);

	ISoundSource* loadSound(string name, const ik_c8* path, float default_volume = 1.0f);
	vector<SubSystem*> sub_systems;
	ISoundSource* getSoundSource(string name);
	void play2D(string name);

	
private:
	ISoundEngine* se;
	map<string, ISoundSource*> audio;
};

