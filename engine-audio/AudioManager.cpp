#include "AudioManager.h"


AudioManager::AudioManager()
{
	// Create the sound engine
	se = createIrrKlangDevice();
	// check for errors with creation
	if (!se) {
		cout << "Error: Could not create Sound Engine" << endl;
	}
}


AudioManager::~AudioManager()
{
}

void AudioManager::update(float msec){
	for (int i = 0; i < sub_systems.size(); i++){
		sub_systems[i]->update(msec);
	}
}

void AudioManager::destroy(){

}

ISoundSource* AudioManager::loadSound(string name, const ik_c8* path, float default_volume) {

	// insert into map, can be accessed later with the name
	audio.insert(pair<string, ISoundSource*>(name, se->addSoundSourceFromFile(path)));
	// set the default volume if specified- 1.0 by default
	audio[name]->setDefaultVolume(default_volume);

	return audio[name];
	
}

void AudioManager::init(){

	for (int i = 0; i < sub_systems.size(); i++) {
		sub_systems[i]->init();
	}
}

ISoundSource* AudioManager::getSoundSource(string name) {
	return audio[name];
}

void AudioManager::addSubSystem(SubSystem* ss) {
	sub_systems.push_back(ss);
}

void AudioManager::play2D(string name) {
	se->play2D(audio[name]);
}