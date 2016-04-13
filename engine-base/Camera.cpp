#include "Camera.h"

/*
Polls the camera for keyboard / mouse movement.
Should be done once per frame! Pass it the msec since
last frame (default value is for simplicities sake...)
*/


Matrix4 Camera::viewMatrix;
Matrix4 Camera::projMatrix;


void Camera::destroy() {};

void Camera::update(float msec)	{
	/*
	//Update the mouse by how much
	pitch -= (Mouse::GetRelativePosition().y);
	yaw	  -= (Mouse::GetRelativePosition().x);

	//Bounds check the pitch, to be between straight up and straight down ;)
	pitch = min(pitch,90.0f);
	pitch = max(pitch,-90.0f);

	if(yaw <0) {
		yaw+= 360.0f;
	}
	if(yaw > 360.0f) {
		yaw -= 360.0f;
	}
*/
	msec *= 2.0f;

	if (Keyboard::KeyDown(KEY_W)) position += Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(0, 0, -1) * msec*0.2;
	if (Keyboard::KeyDown(KEY_S)) position -= Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(0, 0, -1) * msec*0.2;

	if (Keyboard::KeyDown(KEY_A)) position += Matrix4::Rotation(yaw, Vector3(0,1,0)) * Vector3(-1,0,0) * msec*0.2;
	if (Keyboard::KeyDown(KEY_D)) position -= Matrix4::Rotation(yaw, Vector3(0,1,0)) * Vector3(-1,0,0) * msec*0.2;

	if (Keyboard::KeyDown(KEY_SHIFT)) position.y += msec*0.2;
	if (Keyboard::KeyDown(KEY_SPACE)) position.y -= msec*0.2;
	this->viewMatrix = this->BuildViewMatrix();
}


/*
Generates a view matrix for the camera's viewpoint. This matrix can be sent
straight to the shader...it's already an 'inverse camera' matrix.
*/
Matrix4 Camera::BuildViewMatrix()	{
	//Why do a complicated matrix inversion, when we can just generate the matrix
	//using the negative values ;). The matrix multiplication order is important!
	return	Matrix4::Rotation(-pitch, Vector3(1,0,0)) * 
			Matrix4::Rotation(-yaw, Vector3(0,1,0)) * 
			Matrix4::Translation(-position);
};


/*
If you've read through the Deferred Rendering tutorial you should have a pretty
good idea what this function does. It takes a 2D position, such as the mouse
position, and 'unprojects' it, to generate a 3D world space position for it.

Just as we turn a world space position into a clip space position by multiplying
it by the model, view, and projection matrices, we can turn a clip space
position back to a 3D position by multiply it by the INVERSE of the
view projection matrix (the model matrix has already been assumed to have
'transformed' the 2D point). As has been mentioned a few times, inverting a
matrix is not a nice operation, either to understand or code. But! We can cheat
the inversion process again, just like we do when we create a view matrix using
the camera.

So, to form the inverted matrix, we need the aspect and fov used to create the
projection matrix of our scene, and the camera used to form the view matrix.

*/
Vector3	Camera::UnProject(Vector3 position, float aspect, float fov) {
	//Create our inverted matrix! Note how that to get a correct inverse matrix,
	//the order of matrices used to form it are inverted, too.
	Matrix4 invVP = GenerateInverseView() * GenerateInverseProjection(aspect, fov);

	//Our mouse position x and y values are in 0 to screen dimensions range,
	//so we need to turn them into the -1 to 1 axis range of clip space.
	//We can do that by dividing the mouse values by the width and height of the
	//screen (giving us a range of 0.0 to 1.0), multiplying by 2 (0.0 to 2.0)
	//and then subtracting 1 (-1.0 to 1.0).
	Vector4 clipSpace = Vector4(
		(position.x / (float)width) * 2.0f - 1.0f,
		(position.y / (float)height) * 2.0f - 1.0f,
		(position.z) - 1.0f,
		1.0f
		);

	//Then, we multiply our clipspace coordinate by our inverted matrix
	Vector4 transformed = invVP * clipSpace;

	//our transformed w coordinate is now the 'inverse' perspective divide, so
	//we can reconstruct the final world space by dividing x,y,and z by w.
	return Vector3(transformed.x / transformed.w, transformed.y / transformed.w, transformed.z / transformed.w);
}


/*
Here's how we generate an 'inverse' projection. It's very similar to the
Matrix::Perpsective function (in fact, it'll only work with perspective
matrices, but why would we use anything else!), but with some inverted
values (and a couple of hard coded 'inversions')

This function only holds true when znear < zfar and znear > 0.0f (both
sensible restrictions!)

Technically we should have a znear and zfar parameter for this, but
as long as the above is true, this cheated matrix is 'good enough'.
*/
Matrix4 Camera::GenerateInverseProjection(float aspect, float fov) {
	Matrix4 m;

	const float h = 1.0f / tan(fov*PI_OVER_360);

	m.values[0] = aspect / h;
	m.values[5] = tan(fov*PI_OVER_360);
	m.values[10] = 0.0f;

	m.values[11] = -0.5f;
	m.values[14] = -1.0f;
	m.values[15] = 0.5f;

	return m;
}

/*
And here's how we generate an inverse view matrix. It's pretty much
an exact inversion of the BuildViewMatrix function of the Camera class!
*/
Matrix4 Camera::GenerateInverseView() {
	float pitch = this->GetPitch();
	float yaw = this->GetYaw();
	Vector3 position = this->GetPosition();

	Matrix4 iview =
		Matrix4::Translation(position) *
		Matrix4::Rotation(yaw, Vector3(0, -1, 0)) *
		Matrix4::Rotation(pitch, Vector3(-1, 0, 0));

	return iview;
}

/*
Here's how we generate the 3D normalised direction vector that we
use with our picking ray. We use our new unproject function /twice/
and then normalise the result. Here's how this works! We unproject
once right up against the near plane, and then once again, right
up against the far plane. This gives us 2 positions in space, both
'under' the mouse pointer from the camera's perspective. We can
then get the direction vector between them and normalise it,
giving us a direction vector that comes from our camera position,
and passes through the mouse pointer.

Near				Far
|					|
|					|
|					|
O->		|*				   *|
|					|
|					|
*/
/*
Vector3 Camera::GetMouseDirectionVector3(float aspect, float fov) {
	Vector2 mpos = Window::GetOSMousePosition();

	//We remove the y axis mouse position from height as OpenGL is 'upside down',
	//and thinks the bottom left is the origin, instead of the top left!
	Vector3 nearPos = Vector3(mpos.x,
		height - mpos.y,
		0.0f
		);

	//We also don't use exactly 1.0 (the normalised 'end' of the far plane) as this
	//causes the unproject function to go a bit weird. 
	Vector3 farPos = Vector3(mpos.x,
		height - mpos.y,
		0.99999999
		);

	Vector3 a = UnProject(nearPos, aspect, fov);
	Vector3 b = UnProject(farPos, aspect, fov);
	Vector3 c = b - a;

	c.Normalise();

	return c;
}

*/


