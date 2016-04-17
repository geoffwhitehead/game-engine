#include "PhysicsObject.h"


PhysicsObject::PhysicsObject(b2World* world, Vector3 start_pos, float pixels_per_m ){
	this->start_pos = start_pos;
	this->renderObject = nullptr;
	this->pixels_per_m = pixels_per_m;
	this->world = world;
}

PhysicsObject::PhysicsObject(b2World* world, Vector3 start_pos, RenderObject *r, float pixels_per_m ){
	this->start_pos = start_pos;
	this->renderObject = r;
	this->pixels_per_m = pixels_per_m;
	this->world = world;
}

void PhysicsObject::updateRenderObject() {
	this->renderObject->SetModelMatrix(Matrix4::Translation(Vector3(body->GetPosition().x, body->GetPosition().y, start_pos.z)));
};

PhysicsObject::~PhysicsObject(){
}

Vector3& PhysicsObject::getPos() { 
	return Vector3(body->GetPosition().x, body->GetPosition().y, start_pos.z);
}


void PhysicsObject::update(float dt) {
	updateRenderObject();
};



void PhysicsObject::createFixture(b2Shape* shape, bool is_sensor) {
	// note that friction, etc. can be modified later by looping
	// over the body's fixtures and calling fixture->SetFriction()
	b2FixtureDef fixtureDef;
	fixtureDef.shape = shape;
	if (is_sensor) {
		fixtureDef.isSensor = true;
	}
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.7f;
	fixtureDef.restitution = 0.1f;
	fixtureDef.filter.categoryBits = eFilterNonSolid;
	fixtureDef.filter.maskBits = 0xffff;
	this->body->CreateFixture(&fixtureDef);
}



void PhysicsObject::addCircularFixtureToBody(float radius, bool is_sensor) {
	b2CircleShape shape;
	shape.m_radius = radius * this->pixels_per_m;
	this->createFixture(&shape, is_sensor);
}

void PhysicsObject::addRectangularFixtureToBody(float width, float height, bool is_sensor) {
	b2PolygonShape shape;
	shape.SetAsBox(
		width * this->pixels_per_m,
		height * this->pixels_per_m
	);
	this->createFixture(&shape, is_sensor);
}

void PhysicsObject::addBodyToWorld(b2World* world, bool is_dynamic) {
	
	this->b2_world = world;
	b2BodyDef bodyDef;
	bodyDef.position.Set(
		(float32)start_pos.x / pixels_per_m,
		(float32)start_pos.y / pixels_per_m
	);
	if (is_dynamic) {
		bodyDef.type = b2_dynamicBody;
	}
	bodyDef.userData = this;
	bodyDef.fixedRotation = true;
	this->body = b2_world->CreateBody(&bodyDef);
}