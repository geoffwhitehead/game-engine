#pragma once
#include <vector>
#include <string>
#include "../nclgl/Vector3.h"
#include "../engine-base/Mesh.h"
#include "../engine-base/Shader.h"
#include "../engine-base/RenderObject.h"
#include "../engine-base/PhysicsObject.h"
#include "Renderer.h"

using namespace std;

class Entity
{
public:
	Entity();
	Entity(string name, string str_parent, string group, string sub_group, Vector3 pos, Vector3 acc, Vector3 vel, Mesh* mesh, Shader* shader, GLuint texture);
	Entity(string name, string str_parent, string group, string sub_group, Vector3 pos, Vector3 acc, Vector3 vel, Mesh* mesh, Shader* shader);
	Entity(string name, string str_parent, string group, string sub_group, Vector3 pos, Vector3 acc, Vector3 vel);

	virtual ~Entity();

	void setPos(Vector3);
	void setAcc(Vector3);
	void setVel(Vector3);
	void setMass(float);

	

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
	bool is_collidable = true;
	bool is_renderable = true;

private:
	PhysicsObject* physicsObject;
	RenderObject* renderObject;
	Entity*	e_parent;
	vector<Entity*> children;
	
	float dt;
};

