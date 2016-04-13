#pragma once

#include "../nclgl/Matrix4.h"
#include "Mesh.h"
#include "Shader.h"
#include "../nclgl/OGLRenderer.h"
#include "Camera.h"

class RenderObject	{
public:
	RenderObject(void);
	RenderObject(Vector3 pos, Mesh*m, Shader*s, GLuint t = 0);
	~RenderObject(void);

	Mesh*	GetMesh()	const			{return mesh;}
	void	SetMesh(Mesh*m)				{mesh = m;}

	Shader* GetShader()		const		{return shader; }
	void	SetShader(Shader*s)			{shader = s;}

	GLuint	GetTexture()		const	{return texture;}
	void	SetTexture(GLuint tex)		{texture = tex;}

	void	SetModelMatrix(Matrix4 mat) {modelMatrix = mat;}
	Matrix4 GetModelMatrix()	const	{return modelMatrix;}

	virtual void update(float msec);

	virtual void Draw() const;

	//virtual void Update(float msec);


	Matrix4 GetWorldTransform() const {
		return worldTransform;
	}

	void setParent(RenderObject* r);
	void UpdateShaderMatrices();
protected:

	Mesh*	mesh;
	Shader*	shader;

	GLuint	texture;

	Matrix4 modelMatrix;
	Matrix4 worldTransform;

	float particleSize;
	RenderObject* parent;
};

