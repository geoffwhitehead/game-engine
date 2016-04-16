
#include "Entity.h"

Entity::Entity(){
	this->name = "default";
	this->renderObject = nullptr;
}

Entity::Entity(string name, string str_parent, string group, string sub_group, Vector3 pos, Mesh* mesh, Shader* shader, GLuint texture, bool is_renderable, bool is_physical, float pixels_per_m, b2World* world){
	
	this->name = name;
	this->group = group;
	this->str_parent = str_parent;
	this->sub_group = sub_group;
	this->renderObject = new RenderObject(pos, mesh, shader, texture);
	this->physicsObject = new PhysicsObject(world, pos, renderObject, pixels_per_m);
	this->is_renderable = is_renderable;
	this->is_physical = is_physical;
}

Entity::Entity(string name, string str_parent, string group, string sub_group, Vector3 pos, Mesh* mesh, Shader* shader, bool is_renderable, bool is_physical, float pixels_per_m, b2World* world){
	this->name = name;
	this->str_parent = str_parent;
	this->group = group;
	this->sub_group = sub_group;
	this->renderObject = new RenderObject(pos, mesh, shader);
	this->physicsObject = new PhysicsObject(world, pos, renderObject, pixels_per_m);
	this->is_renderable = is_renderable;
	this->is_physical = is_physical;
}

Entity::Entity(string name, string str_parent, string group, string sub_group, Vector3 pos, float pixels_per_m, b2World* world){
	this->name = name;
	this->str_parent = str_parent;
	this->group = group;
	this->sub_group = sub_group;
	this->renderObject = nullptr;
	this->physicsObject = new PhysicsObject(world, pos, pixels_per_m);
	this->is_renderable == false;
	this->is_physical = true;
}

//destuctor
Entity::~Entity()
{
	if (renderObject) delete renderObject;
}

RenderObject* Entity::getRenderObject() const {
	return this->renderObject;
}

void Entity::update(float dt){
	
	this->renderObject->update(dt);
	if (is_physical) {
			this->physicsObject->update(dt);
	}
	
	for (vector<Entity*>::iterator i = children.begin(); i != children.end(); ++i)
		(*i)->update(dt);	

}

void Entity::render(Renderer* renderer){
	
	if (is_renderable){
		renderer->render(renderObject);
	}

	for (vector<Entity*>::iterator child = children.begin(); child != children.end(); ++child)
		(*child)->render(renderer);
}

void Entity::addChild(Entity *child) {
	children.push_back(child);
	child->e_parent = this;
	child->renderObject->setParent(this->renderObject);
}
vector<Entity*> Entity::getChildren(){
	return this->children;
}

PhysicsObject* Entity::getPhysicsObject() const {
	return physicsObject;
}
