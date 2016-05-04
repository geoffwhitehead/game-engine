#version 150 core

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
	
} OUT;

void main(void)	{
	float mod = time;
	vec4 worldPos = modelMatrix * vec4(position, 1.0);
	worldPos.y += mod;
	gl_Position		= (projMatrix * viewMatrix * worldPos);
	OUT.texCoord	= texCoord;
	OUT.colour		= colour;
}