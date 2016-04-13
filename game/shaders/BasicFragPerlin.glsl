#version 150 core
uniform sampler2D perlinTex;
uniform float time;

in Vertex	{
	smooth vec4 colour;
	smooth vec2 texCoord; 	
} IN;

out vec4 gl_FragColor;

void main(void)	{	
	vec4 perlinColour = texture(perlinTex, IN.texCoord);
	perlinColour.a = perlinColour.r * time;
	
	gl_FragColor = perlinColour;
}