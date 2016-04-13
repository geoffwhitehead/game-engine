#version 150 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in Vertex {
	vec2 texCoord;
	vec3 worldPos;
	
} IN[];

out Vertex {
	vec3 normal;
	vec2 texCoord;
	vec3 worldPos;
} OUT;

void main(){
	vec3 ba = IN[1].worldPos - IN[0].worldPos;
	vec3 ca = IN[2].worldPos - IN[0].worldPos;
	
	OUT.normal = normalise(cross(ba, ca));
	for (int i = 0; i < gl_in.length(); ++i) {
		gl_Position = gl_in[i].gl_Position;
		
		OUT.texCoord = IN[i].texCoord;
		OUT.worldPos = IN[i].worldPos;
		
		EmitVertex();
	}
	EndPrimitive();
}