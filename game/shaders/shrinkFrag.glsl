#version 330 core
uniform sampler2D smileyTex;
uniform float time;

in Vertex	{
	smooth vec4 colour;
	smooth vec2 texCoord; 	
} IN;

out vec4 gl_FragColor;

void main(void)	{	
	
	vec4 smileyColour = texture(smileyTex, IN.texCoord);
	
	gl_FragColor = smileyColour;
}