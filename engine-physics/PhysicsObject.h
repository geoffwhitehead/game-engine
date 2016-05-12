#pragma once
#include <vector>
#include "RenderObject.h"
#include <Box2D\Box2D.h>


/*
*	Class: PhysicsObjects
*	Author:	Geoff Whitehead
*	Description: Contains physics related information on an entity. Contains a reference to a box 2d body 
		which is the gateway to all box2d related data and information on this entity.
*/


class PhysicsObject
{
private:
	RenderObject* renderObject;
	PhysicsObject* parent;
	

	vector<PhysicsObject*> children;
	void updateRenderObject();
	enum {
		eFilterLevel = 0x01,
		eFilterSolid = 0x02,
		eFilterNonSolid = 0x04
	};
public:
	b2World* b2_world;

	PhysicsObject(b2World* world, Vector3 start_pos, float pixels_per_m );
	PhysicsObject(b2World* world, Vector3 start_pos, RenderObject *r, float pixels_per_m );
	~PhysicsObject();

	Vector3 getPos() ;

	void update(float);

	Vector3 start_pos;
	float pixels_per_m;
	b2World* world;

	void addCircularFixtureToBody(float radius, bool is_sensor);
	void addRectangularFixtureToBody(float width, float height, bool is_sensor);
	void createFixture(b2Shape* shape, bool is_sensor);
	void addBodyToWorld(b2World* world, bool is_dynamic);


	b2Body* body;
	float pos_z;
};