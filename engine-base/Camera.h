/******************************************************************************
Class:Camera
Implements:
Author:Rich Davison	<richard.davison4@newcastle.ac.uk>
Description:FPS-Style camera. Uses the mouse and keyboard from the Window
class to get movement values!

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../nclgl/Window.h"
#include "../nclgl/Matrix4.h"
#include "../nclgl/Vector3.h"
#include "SubSystem.h"

class Camera : public SubSystem {
public:


	static Matrix4 viewMatrix;
	static Matrix4 projMatrix;

	void Camera::init(){};

	Camera(void){
		yaw		= 0.0f;
		pitch	= 0.0f;
	};

	Camera(float pitch, float yaw, Vector3 position, float height, float width){
		this->pitch		= pitch;
		this->yaw		= yaw;
		this->position	= position;
		this->height	= height;
		this->width		= width;
	}

	~Camera(void){};

	void update(float msec = 10.0f);

	void destroy();

	//Builds a view matrix for the current camera variables, suitable for sending straight
	//to a vertex shader (i.e it's already an 'inverse camera matrix').
	Matrix4 BuildViewMatrix();

	//Gets position in world space
	Vector3 GetPosition() const { return position;}
	//Sets position in world space
	void	SetPosition(Vector3 val) { position = val;}

	//Gets yaw, in degrees
	float	GetYaw()   const { return yaw;}
	//Sets yaw, in degrees
	void	SetYaw(float y) {yaw = y;}

	//Gets pitch, in degrees
	float	GetPitch() const { return pitch;}
	//Sets pitch, in degrees
	void	SetPitch(float p) {pitch = p;}

	Vector3	UnProject(Vector3 position, float aspect, float fov);
	Matrix4 GenerateInverseProjection(float aspect, float fov);
	Matrix4 GenerateInverseView();
	//Vector3 GetMouseDirectionVector3(float aspect, float fov);



protected:
	float	yaw;
	float	pitch;
	Vector3 position;
	float width;
	float height;
};