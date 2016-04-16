#pragma once
#include "../nclgl/OGLRenderer.h"

#include "RenderObject.h"
#include <ctime>
#include <cstdlib>
#include <vector>
#include <ctime>
#include <cstdlib>




using std::vector;

class Renderer : public OGLRenderer	{
public:
	Renderer(Window &parent);
	~Renderer(void);
	virtual void	render(RenderObject *o);

protected:

};

