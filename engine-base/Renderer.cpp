#include "Renderer.h"


Renderer::Renderer(Window &parent) : OGLRenderer(parent)	{
	
	// OPENGL SETTINGS 
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	srand(std::time(NULL));

}

Renderer::~Renderer(void)	{
	
}

void	Renderer::render(RenderObject *o) {
	if (o->GetShader() && o->GetMesh()) {
		GLuint program = o->GetShader()->GetShaderProgram();
		glUseProgram(program);
		o->UpdateShaderMatrices();

		o->Draw();
	}
}

				   