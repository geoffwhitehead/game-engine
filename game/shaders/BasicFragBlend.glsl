#version 150 core
uniform sampler2D smileyTex;
uniform sampler2D staticTex;
uniform float time;

in Vertex	{
	smooth vec4 colour;
	smooth vec2 texCoord; 	
} IN;

out vec4 gl_FragColor;

void main(void)	{	
	vec4 smileyColour = texture(smileyTex, IN.texCoord);
	vec4 noiseColour = texture(staticTex, IN.texCoord);
	
	vec2 tempTex = IN.texCoord;
	tempTex.y += time;
	
	vec4 mix = mix(smileyColour, noiseColour, time);
	
	gl_FragColor = mix;
}