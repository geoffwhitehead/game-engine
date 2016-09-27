# Lightweight OpenGL 2D Game Engine

A lightweight game engine i created in OpenGL. It uses the NCLGL framework, JsonCPP, Klang Audio, and Box2d.

## Getting Started

Clone and run the engine with Visual Studio. It contains a copy of the moonbase commander strategy game i created to demo the engine. Check the game/ folder to get an idea of how to use the engine.

### Installing

To create a game you will need to extend the engine base classes in order to use their functionality.
Inlude the following base classes and then instantiate them in the main class for your game:

```
#include "../engine-base/Camera.h"
#include "../engine-base/GameManager.h"
#include "../engine-audio/AudioManager.h"
#include "../engine-input/InputManager.h"
#include "../engine-base/GameLogicManager.h"
#include "../engine-physics/PhysicsManager.h"
```

These are known as system "system managers". Each "system manager" can have "sub systems" which you can register with it. Sub systems must use the base class SubSystem and can contain all your game code.

```
First initiate the system Manager.
GameManager *gm = new GameManager(W_X, W_Y); // provide the screen size X and Y
```
You can then instantiate all the other system managers shown above.
eg.

```
GameLogicManager* glm = new GameLogicManager();
```
Then add the instantiated systemManagers to the game manager
```
gm->addSystemManager(glm);
```

Excluding the gameManager, each manager contains a function called addSubSystem(). You can create game classes that have "SubSytem" as their base class and then register them with the manager using the addSubSystem() method. By doing this, your game classes will be called once per frame.
```
class GameLogic :
	public SubSystem
```
You will have to create certain methods such as init() when you do this.

Then instantiate and register your created game classes to its manager. eg.

```
GameLogic* gl = new GameLogic(gm, glm, pm->b2_world, am, camera); // in this case (shown in the game demo) my GameLogic class requires a reference to the GameManager, GameLogicManager, the box2d world, AudioManager, and the camera instance.

glm->addSubSystem(gl);
```

Call GameManager()->run() to start the game.

```
gm->run();
```

## Built With

* Visual Studio 2015
* Box2D
* GLEW
* SOIL
* irrKlang
* jsonCPP
* NCLGL

## Authors

* **Geoff Whitehead - (https://github.com/geoffwhitehead)

## License

This project is licensed under the MIT License - see the [LICENSE.md](./LICENSE.md) file for details
