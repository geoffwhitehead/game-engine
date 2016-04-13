#version 400 core

layout (triangles, cw) in; 

uniform sampler2D heightMap;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

in Vertex { // sent from the TCS
	vec2 texCoord;
} IN[];	// equal to the TCS layout size

out Vertex {	// each TES works on a single vertex
	vec2 texCoord;
} OUT;

void main() {
	vec3 p0 = gl_TessCoord.x * vec3(IN[0].texCoord, 1);
    vec3 p1 = gl_TessCoord.y * vec3(IN[1].texCoord, 1);
    vec3 p2 = gl_TessCoord.z * vec3(IN[2].texCoord, 1);
	vec3 worldPos = normalize(p0 + p1 + p2);
    
	float height = texture(heightMap, OUT.texCoord).x;
	worldPos.y += height * 2;
	
	gl_Position = projMatrix * viewMatrix * vec4(worldPos, 1);
	
 
	
	
	
}