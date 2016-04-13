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
	vec3 normal;
} OUT;

void main(void)	{

	vec3 norm = glNormal.xyz; 

	vec4 worldPos = modelMatrix * vec4(position, 1.0);
	gl_Position		= (projMatrix * viewMatrix * worldPos);
	
	mat3 normalMatrix = mat3(transpose(inverse(viewMatrix * modelMatrix)));
	mat4 modelView = viewMatrix * modelMatrix;
	
	OUT.normal =  modelView * vec4(norm, 0.0);
	OUT.texCoord	= texCoord;
	OUT.colour		= colour;
}