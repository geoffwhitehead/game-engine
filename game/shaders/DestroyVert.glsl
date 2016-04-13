#version 150 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

in  vec3 position;
in  vec2 texCoord;
in  vec4 colour;

out Vertex	{
	vec2 texCoord;
	vec4 colour;
} OUT;

void main(void)	{
	vec4 worldPos = modelMatrix * vec4(position, 1.0);
	gl_Position		= (projMatrix * viewMatrix * worldPos);
	
	OUT.texCoord	= texCoord;
	OUT.colour		= colour;
}