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
#include "GameIO.h"

#include <iostream>
#include <map>



#define W_X 1024.0f
#define W_Y 768.0f

void main(void) {

	

	// GAME MANAGER
	GameManager *gm = new GameManager(W_X, W_Y);

	//ADD SYSTEM MANAGERS
	
	CollisionManager* cm = new CollisionManager();
	AudioManager* am = new AudioManager();
	InputManager* im = new InputManager();
	EventManager* em = new EventManager();
	GameLogicManager* glm = new GameLogicManager();

	//CREATE SUB SYSTEMS
	GameIO* gio = new GameIO(cm, "./levels/");
	gm->addFileInput(gio);
	gm->loadLevel("data.json");

	GameEvents* ge = new GameEvents(gm);

	Camera* camera = new Camera(0.0f, 0.0f, Vector3(0, 0, 400), W_X, W_Y);
	Camera::projMatrix = Matrix4::Orthographic(1, 1000, W_X/4.0f, -W_X/4.0f, W_Y/4.0f, -W_Y/4.0f);
	//Camera::viewMatrix = Matrix4::BuildCamera(Vector3(0.0, 50.0, 20.0), Vector3(0.0, 0.0, 0.0));
		
		//camera->BuildViewMatrix();
	
	GameInput* gi = new GameInput(gm, ge, camera);
	GameAudio* ga = new GameAudio(am, ge);
	CollisionResponse* cr = new CollisionResponse(cm, ge);
	GameLogic* gl = new GameLogic(glm, ge);
	
	
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