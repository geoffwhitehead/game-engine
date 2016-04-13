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
	
	if(smileyColour.b > 0.1){
		vec2 tempTex = IN.texCoord;
		tempTex.y += time;
		gl_FragColor = texture(staticTex, tempTex);
		
	} else {
		gl_FragColor = smileyColour;
	}
}