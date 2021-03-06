#include "AudioManager.h"


AudioManager::AudioManager()
{
	createSoundDevice();
}

AudioManager::AudioManager(IOManager* iom) {
	this->iom = iom;
	createSoundDevice();
}
ISoundEngine* AudioManager::getSoundEngine() {
	return se;
}
void AudioManager::createSoundDevice() {
	// Create the sound engine
	se = createIrrKlangDevice();
	// check for errors with creation
	if (!se) {
		cout << "Error: Could not create Sound Engine" << endl;
	}
}

AudioManager::~AudioManager()
{
	// dropping will handle deletion of all sounds
	se->drop();
}

void AudioManager::update(float msec){
	for (int i = 0; i < sub_systems.size(); i++){
		sub_systems[i]->update(msec);
	}
}

void AudioManager::destroy(){
	// dropping will handle deletion of all sounds
	for (int i = 0; i < sub_systems.size(); i++) {
		delete sub_systems[i];
	}
	se->drop();
}

// clear sounds from memory, used for loading new level and new sounds
void AudioManager::clear() {
	se->removeAllSoundSources();
}

ISoundSource* AudioManager::loadSound(string name, const ik_c8* path, float default_volume) {

	// insert into map, can be accessed later with the name
	audio.insert(pair<string, ISoundSource*>(name, se->addSoundSourceFromFile(path)));
	// set the default volume if specified- 1.0 by default
	audio[name]->setDefaultVolume(default_volume);

	return audio[name];
	
}

void AudioManager::init(){
	
	// if class was initialised with file input
	if (iom) {
		loudAudioFromFile();
	}

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

// Load all sounds contained in vector from class input manager that contains details on files to load.
void AudioManager::loudAudioFromFile() {
	for (int i = 0; i < iom->in_audio.size(); i++) {
		loadSound(iom->in_audio[i]->name, iom->in_audio[i]->path.c_str(), iom->in_audio[i]->default_volume);
	}
}