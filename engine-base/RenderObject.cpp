#include "RenderObject.h"

RenderObject::RenderObject()	{
	mesh	= NULL;
	shader	= NULL;
	texture = NULL;
	parent  = NULL;
	modelMatrix = Matrix4::Translation(Vector3(0,0,0));
}

RenderObject::RenderObject(Vector3 pos, Mesh*m, Shader*s, GLuint t) {
	mesh	= m;
	shader	= s;
	texture = t;
	parent  = NULL;
	modelMatrix = Matrix4::Translation(pos);
}

RenderObject::~RenderObject(void){

}

void RenderObject::setParent(RenderObject* r){
	this->parent = r;
}

void RenderObject::update(float msec) {

	if(parent) {
		worldTransform = parent->modelMatrix * modelMatrix;
		//worldTransform = modelMatrix * parent->modelMatrix;
	}
	else {
		worldTransform = modelMatrix;
	}
}

void RenderObject::Draw() const {
	if(mesh) {
		mesh->Draw();
	}
}

void RenderObject::UpdateShaderMatrices()	{
	glUniformMatrix4fv(glGetUniformLocation(shader->GetShaderProgram(), "modelMatrix"), 1, false, (float*)&modelMatrix);
	glUniformMatrix4fv(glGetUniformLocation(shader->GetShaderProgram(), "viewMatrix"), 1, false, (GLfloat*)&Camera::viewMatrix);
	glUniformMatrix4fv(glGetUniformLocation(shader->GetShaderProgram(), "projMatrix"), 1, false, (GLfloat*)&Camera::projMatrix);

	//glUniformMatrix4fv(glGetUniformLocation(shader->GetShaderProgram(), "textureMatrix"), 1, false, (float*)&textureMatrix);
}