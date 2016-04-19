#pragma once
#include <vector>
#include <string>
#include "../nclgl/Vector3.h"
#include "../engine-base/Mesh.h"
#include "../engine-base/Shader.h"
#include "../engine-base/RenderObject.h"
#include "Renderer.h"
#include <Box2D\Box2D.h>
#include "../engine-physics/PhysicsObject.h"

/*
*	Class: Entity
*	Author:	Geoff Whitehead
*	Description: Base class which holds references to render and physics objects and 
*		various other functions.
*/

using namespace std;

class Entity
{
public:
	Entity();
	Entity(string name, string str_parent, string group, string sub_group, Vector3 pos, Mesh* mesh, Shader* shader, GLuint texture, bool is_renderable, bool is_physical, float pixels_per_m, b2World* world);
	Entity(string name, string str_parent, string group, string sub_group, Vector3 pos, Mesh* mesh, Shader* shader, bool is_renderable, bool is_physical, float pixels_per_m, b2World* world);
	
	virtual ~Entity();

	RenderObject* getRenderObject() const;
	PhysicsObject* getPhysicsObject() const;

	inline void translate(Vector3 pos){ this->renderObject->SetModelMatrix(this->renderObject->GetModelMatrix() * Matrix4::Translation(pos)); };
	inline void scale(Vector3 scale){ this->renderObject->SetModelMatrix(this->renderObject->GetModelMatrix() * Matrix4::Scale( scale)); };
	inline void rotate(float degree, Vector3 rotation){ this->renderObject->SetModelMatrix(this->renderObject->GetModelMatrix() * Matrix4::Rotation(degree, rotation)); };
	
	void update(float);

	void render(Renderer*);

	void addChild(Entity *child);
	vector<Entity*> getChildren();
	string name;
	string group;
	string sub_group;
	string str_parent; // need this to hold the result from the tet
	bool is_physical;
	bool is_renderable;



protected:

	RenderObject* renderObject;
	PhysicsObject* physicsObject;
	Entity*	e_parent;
	vector<Entity*> children;
	
	float dt;
};

