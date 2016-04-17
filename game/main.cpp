#include "../nclgl/OGLRenderer.h"

#include "../engine-base/Camera.h"
#include "../engine-base/GameManager.h"
#include "../engine-audio/AudioManager.h"
#include "../engine-input/InputManager.h"
#include "../engine-base/GameLogicManager.h"
#include "../engine-physics/PhysicsManager.h"

#include "ContactListener.h"
#include "GameInput.h"
#include "GameAudio.h"
#include "GameEvents.h"
#include "GameLogic.h"
#include "GameIO.h"

#include <iostream>
#include <map>



#define W_X 1600.0f
#define W_Y 900.0f


const float pixels_per_m = 32.0f;
//const float gravity = -pixels_per_m / 0.7f; // adjust
const float gravity = 0.0f;


void main(void) {

	// GAME MANAGER
	GameManager *gm = new GameManager(W_X, W_Y);

	// PHYSICS
	PhysicsManager* pm = new PhysicsManager(gm, gravity, pixels_per_m);

	// FILE IO
	GameIO* gio = new GameIO("./levels/", pm->b2_world, pm->pixels_per_m);
	gm->addFileInput(gio);
	gm->loadLevel("level_1.json");

	// CAMERA
	Camera* camera = new Camera(0.0f, 0.0f, Vector3(0, 0, 400), W_X, W_Y);
	Camera::projMatrix = Matrix4::Orthographic(1, 1000, W_X / 4.0f, -W_X / 4.0f, W_Y / 4.0f, -W_Y / 4.0f);
	//Camera::viewMatrix = Matrix4::BuildCamera(Vector3(0.0, 50.0, 20.0), Vector3(0.0, 0.0, 0.0));
	//camera->BuildViewMatrix();

	//ADD SYSTEM MANAGERS
	AudioManager* am = new AudioManager(gio); // init with file input to cause it to load audio from file
	InputManager* im = new InputManager();
	GameLogicManager* glm = new GameLogicManager();
	
	//CREATE SUB SYSTEMS
	GameLogic* gl = new GameLogic(gm, glm, pm->b2_world, am, camera);
	GameInput* gi = new GameInput(gl, camera);
	ContactListener* cl = new ContactListener(gl);
	GameAudio* ga = new GameAudio(gl, am); // created to seperate audio from game
	
	//register managers
	gm->addSystemManager(am);
	pm->addListener(cl);
	gm->addSystemManager(im);
	gm->addSystemManager(glm);
	gm->addSystemManager(pm);

	//register sub systems
	im->addSubSystem(camera);
	glm->addSubSystem(gl);
	im->addSubSystem(gi);
	am->addSubSystem(ga); // register with audio manager so updates are called
	gm->run();

}