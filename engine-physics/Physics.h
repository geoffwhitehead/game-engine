#pragma once
#include "Windows.h"
#include "../nclgl/Vector3.h"


#define DAMPING_FACTOR 0.99f
#define MINIMUM_VELOCITY 0.0001f
#define COEFF_OF_ELASTICITY 0.8f
#define SRRING_STRENGTH 0.8f 
#define CLAMP 0.09f

using namespace std;


class Physics
{
public:

	Physics();
	~Physics();

	// check that the velocity is above a minimum veolcity or set it to 0; saves cycles when values become imperceptively small
	inline static bool moving(Vector3 &vel) {
		if (abs(vel.x) >= MINIMUM_VELOCITY || abs(vel.y) > MINIMUM_VELOCITY)
			return true;
		else
			vel = Vector3(0.0f, 0.0f, 0.0f);
		return false;
	};

	// if the velocity goes above the CLAMP threshold its lowered to meet the CLAMP value.
	inline static void clamp( Vector3 &vel ) {
		if (vel.Length() > CLAMP) {
			Vector3 unit = vel.getNormal();
			vel = unit * CLAMP;
		}
	}

	// work out the new velocity with a given velocity and elapsed time. 
	inline static Vector3 calcVelocity(Vector3 vel, Vector3 acc, float dt){
		
		// damping factor will gradually reduce the velocity bringing entiies to rest
		return (vel + (acc * dt)) * DAMPING_FACTOR;
		
	}

	// calculate the objects new position based on their velocity and time elapsed
	inline static Vector3 calcDisplacement(Vector3 pos, Vector3 vel, Vector3 acc, float dt){
		
		return ((vel*dt) + (acc * 0.5f * dt*dt));
	}

	inline void explicitEuler(Vector3& pos, Vector3 &vel, Vector3 acc, float dt) {

		if (moving(vel)) {

			vel = (vel + (acc * dt)) * DAMPING_FACTOR;
			clamp(vel);

			pos += calcDisplacement(pos, vel, acc, dt);

		}
	}

	inline void implicitEuler(Vector3& pos, Vector3 &vel, Vector3 acc, float dt) {
		if (moving(vel)) {
			vel = calcVelocity(vel, acc, dt);
			clamp(vel);

			pos += calcDisplacement(pos, vel, acc, dt);

		}
	}

	inline static void semiImplicitEuler(Vector3& pos, Vector3 &vel, Vector3 acc, Vector3& disp, float dt){
		if (moving(vel)) {
			vel = calcVelocity(vel, acc, dt);
			clamp(vel);

			disp = calcDisplacement(pos, vel, acc, dt);
			pos += disp;

		}
	}

	// returns true if the distance between two circles is less than the combines radius
	inline static bool detectCollision(Vector3 pos1, Vector3 pos2, float r1, float r2) {
		float d2 = pow(pos2.x - pos1.x, 2.0) + pow(pos2.y - pos1.y, 2.0) + pow(pos2.z - pos1.z, 2.0);
		if (d2 < (pow((r1 + r2), 2.0)))
			return true;
		return false;
	}

	// if a circle-circle collision occured: determines both objects new velocity
	inline static void resolveCollision(Vector3& pos1, Vector3& pos2, Vector3& vel1, Vector3& vel2, float mass1, float mass2, float r1, float r2){
		
		float d2 = pow(pos2.x - pos1.x, 2.0) + pow(pos2.y - pos1.y, 2.0) + pow(pos2.z - pos1.z, 2.0);

		//calculate penetration depth
		float p = r1 + r2 - sqrt(d2);
			
		//calculate collision normal
		Vector3 N = (pos1 - pos2).getAbs();

		//calculate point of collision
		Vector3 P = pos1 - N * (r1 - p);

		// combined velocity
		Vector3 Vab = vel1 + vel2;

		float VN = Vab.dot(N);

		//float totalForce = -COEFF_OF_ELASTICITY * (vel1 + vel2).dot(N);

		float J = (-(1+COEFF_OF_ELASTICITY) * VN) / (N.dot(N)*((1*mass1) + (1*mass2)));

		float spring_vel = N.dot(Vab);

		//float F = (-SRRING_STRENGTH) * p - (DAMPING_FACTOR * (N.dot(Vab)));

		vel1 = vel1 + (N * (J / mass1));
		vel2 = vel2 - (N * (J / mass2));

	}

	// detects a collision between and cirle and plane
	inline static bool detectCollision(Vector3& c_pos, float r, Vector3 normal, float distance) {
		float result = c_pos.Dot((normal), c_pos) - distance;
		if (abs(result) < r) 
			return true;
		return false;
	}

	// calcuates the new velocity of both the colliding plane and circle.
	inline static void resolveCollision(Vector3& c_pos, Vector3& p_pos, Vector3& c_vel, Vector3& p_vel, float c_mass, float p_mass, float r, Vector3 normal, float distance){
		float result = c_pos.Dot((normal), c_pos) - distance;
		Vector3 N = normal;
		float p = r - result;
		Vector3 P = c_pos - N * (r - p);

		Vector3 Vab = c_vel + p_vel;

		float VN = Vab.dot(N);

		//float totalForce = -COEFF_OF_ELASTICITY * (vel1 + vel2).dot(N);

		float J = (-(1 + COEFF_OF_ELASTICITY) * VN) / ((N.dot(N))*((1*c_mass) + (1*p_mass)));

		c_vel = c_vel + (N * (J / c_mass));
		p_vel = p_vel - (N * (J / p_mass));
	}
};


