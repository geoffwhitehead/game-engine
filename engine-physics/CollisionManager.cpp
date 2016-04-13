#include "CollisionManager.h"


CollisionManager::CollisionManager()
{
}


CollisionManager::~CollisionManager()
{
}

void CollisionManager::init(){
	for (int i = 0; i < sub_systems.size(); i++) {
		sub_systems[i]->init();
	}
}
void CollisionManager::addSubSystem(SubSystem* ss) {
	sub_systems.push_back(ss);
}

void CollisionManager::update(float msec){
	

	// go through collision resolution adn detection procedure
	manageCollisions(msec);
	// update all sub systems
	for (int i = 0; i < sub_systems.size(); i++) {
		sub_systems[i]->update(msec);
	}
	
	// empty the new collisions vector so that the collisions arent resolved multiple times
	clearFrame();
}

void CollisionManager::manageCollisions(float msec) {

	// circle - circle collisions
	for (int i = 0; i < collidableSpheres.size() - 1; i++) {
		//circles
		if (collidableSpheres[i]->is_collidable) {
			for (int j = 1; j < collidableSpheres.size(); j++) {
				if (collidableSpheres[j]->is_collidable){

					if (collidableSpheres[i] != collidableSpheres[j]) {

						// Detect collision
						bool collision = Physics::detectCollision(
							collidableSpheres[i]->getPhysicsObject()->getPos(),
							collidableSpheres[j]->getPhysicsObject()->getPos(),
							(static_cast<Circle*>(collidableSpheres[i]->getPhysicsObject()->getRef())->getRadius()),
							(static_cast<Circle*>(collidableSpheres[j]->getPhysicsObject()->getRef())->getRadius())
						);

						if (collision) {
							// check that a collision hasnt already occured
							if (collision_map[collidableSpheres[i]][collidableSpheres[j]] == false) {
								Physics::resolveCollision(
									collidableSpheres[i]->getPhysicsObject()->getPos(),
									collidableSpheres[j]->getPhysicsObject()->getPos(),
									collidableSpheres[i]->getPhysicsObject()->getVel(),
									collidableSpheres[j]->getPhysicsObject()->getVel(),
									collidableSpheres[i]->getPhysicsObject()->getMass(),
									collidableSpheres[j]->getPhysicsObject()->getMass(),
									(static_cast<Circle*>(collidableSpheres[i]->getPhysicsObject()->getRef())->getRadius()),
									(static_cast<Circle*>(collidableSpheres[j]->getPhysicsObject()->getRef())->getRadius())
								);
								// new collision has occured between these two objects
								collision_map[collidableSpheres[i]][collidableSpheres[j]] = true;
								collision_map[collidableSpheres[j]][collidableSpheres[i]] = true;
								// add this collision to vector to be resolved with game collision logic
								collisions_this_frame.push_back(pair<Entity*, Entity*>(collidableSpheres[i], collidableSpheres[j]));
							}
							else {
								// else do nothing - this collision has already been resolved
							}
						}
						else {
							// else no collision - set both maps to false to mark the objects as not colliding
							collision_map[collidableSpheres[i]][collidableSpheres[j]] = false;
							collision_map[collidableSpheres[j]][collidableSpheres[i]] = false;
						}
					}
				}
			}
		}
	}
	//circle - plane collisions
	for (int i = 0; i < collidableSpheres.size(); i++) {
		if (collidableSpheres[i]->is_collidable) {
			for (int j = 0; j < collidablePlanes.size(); j++) {
				if (collidablePlanes[j]->is_collidable) {
					// Detect collision
					bool collision = Physics::detectCollision(
						collidableSpheres[i]->getPhysicsObject()->getPos(),
						static_cast<Circle*>(collidableSpheres[i]->getPhysicsObject()->getRef())->getRadius(),
						static_cast<Plane*>(collidablePlanes[j]->getPhysicsObject()->getRef())->getNormal(),
						static_cast<Plane*>(collidablePlanes[j]->getPhysicsObject()->getRef())->getDistanceFromOrigin()
					);

					if (collision) {
						// check that a collision hasnt already occured
						if (collision_map[collidableSpheres[i]][collidablePlanes[j]] == false) {
							Physics::resolveCollision(
								collidableSpheres[i]->getPhysicsObject()->getPos(),
								collidablePlanes[j]->getPhysicsObject()->getPos(),
								collidableSpheres[i]->getPhysicsObject()->getVel(),
								collidablePlanes[j]->getPhysicsObject()->getVel(),
								collidableSpheres[i]->getPhysicsObject()->getMass(),
								collidablePlanes[j]->getPhysicsObject()->getMass(),
								static_cast<Circle*>(collidableSpheres[i]->getPhysicsObject()->getRef())->getRadius(),
								static_cast<Plane*>(collidablePlanes[j]->getPhysicsObject()->getRef())->getNormal(),
								static_cast<Plane*>(collidablePlanes[j]->getPhysicsObject()->getRef())->getDistanceFromOrigin()
							);
							// new collision has occured between these two objects
							collision_map[collidableSpheres[i]][collidablePlanes[j]] = true;
							collision_map[collidablePlanes[j]][collidableSpheres[i]] = true;
							// add this collision to vector to be resolved with game collision logic
							collisions_this_frame.push_back(pair<Entity*, Entity*>(collidableSpheres[i], collidablePlanes[j]));
						}

						else {
							// else do nothing - this collision has already been resolved
						}
					}
					else {
						// else no collision - set both maps to false to mark the objects as not colliding
						collision_map[collidableSpheres[i]][collidablePlanes[j]] = false;
						collision_map[collidablePlanes[j]][collidableSpheres[i]] = false;
					}
				}
			}
		}
	}
}

void CollisionManager::destroy(){

}

// add circle to the collision manager to enable it to be factored into collision checks
void CollisionManager::addObject(Entity* entity, float radius){
	Shape* s = new Circle(radius);
	entity->getPhysicsObject()->setRef(s);
	collidableSpheres.push_back(entity);
}

// add plane to the collision manager to enable it to be factored into collision checks
void CollisionManager::addObject(Entity* entity, float distance, Vector3 normal){
	Shape* p = new Plane(distance, normal);
	entity->getPhysicsObject()->setRef(p);
	collidablePlanes.push_back(entity);
}

// remove an entity from the collision manager so that it isnt factored into collision checks
void CollisionManager::removeEntity(Entity* e){
	collidableSpheres.erase(std::remove(collidableSpheres.begin(), collidableSpheres.end(), e), collidableSpheres.end());
	collidablePlanes.erase(std::remove(collidablePlanes.begin(), collidablePlanes.end(), e), collidablePlanes.end());
}

void CollisionManager::clearFrame() {
	collisions_this_frame.clear();
}