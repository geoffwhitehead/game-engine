#pragma once
#include <vector>
#include "RenderObject.h"
#include <Box2D\Box2D.h>

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

	PhysicsObject(b2World* world, Vector3 start_pos, float pixels_per_m);
	PhysicsObject(b2World* world, Vector3 start_pos, RenderObject *r, float pixels_per_m);
	~PhysicsObject();

	Vector3& getPos() ;

	void update(float);

	Vector3 start_pos;
	float pixels_per_m;
	b2World* world;

	void addCircularFixtureToBody(float radius);
	void addRectangularFixtureToBody(float width, float height);
	void createFixture(b2Shape* shape);
	void addBodyToWorld(b2World* world);


	b2Body* body;

};