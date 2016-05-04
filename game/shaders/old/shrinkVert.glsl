#version 330 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

uniform float time;

in  vec3 position;
in  vec2 texCoord;
in  vec4 colour;

out Vertex	{
	vec4 colour;
	vec2 texCoord;
	//vec3 worldPos;
} OUT;

void main(void)	{

	mat4 scale = mat4(1);
	scale[3][3] = 1.0;
	
	vec3 pos = position;
	
	if (time < 0) {
		pos.xyz *= 0;
	} else {
		pos.xyz *= time;
	}
	
	vec4 worldPos = modelMatrix * vec4(pos, 1.0);
	
	gl_Position = projMatrix * viewMatrix * scale * worldPos;
	OUT.texCoord	= texCoord;
	OUT.colour		= colour;
}