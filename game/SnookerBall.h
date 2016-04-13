#pragma once
#include "Entity.h"
class SnookerBall : public Entity
{
public:
	SnookerBall(string name, Vector3 pos, Vector3 acc, Vector3 vel, Mesh* mesh, Shader* shader, GLuint texture, float radius);
	~SnookerBall();
};

