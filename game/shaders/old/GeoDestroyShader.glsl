#version 330 core

uniform float particleSize;
uniform float time;

layout(triangles) in;
layout(triangle_strip, max_vertices = 4) out;


in Vertex	{
	vec2 texCoord;
	vec4 colour;
} IN[];

out Vertex {
	vec2 texCoord;
	vec4 colour;
	
} OUT;

vec4 explode(vec4 position, vec3 normal)
{
    float magnitude = 2.0f;
    vec3 direction = normal * ((sin(time) + 1.0f) / 2.0f) * magnitude; 
    return position + vec4(direction, 0.0f);
} 

vec3 GetNormal()
{
   vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
   vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
   return normalize(cross(a, b));
}  

void main() {	

	vec3 normal = GetNormal();

    gl_Position = explode(gl_in[0].gl_Position, normal);
    OUT.texCoord = IN[0].texCoord;
    EmitVertex();
	
    gl_Position = explode(gl_in[1].gl_Position, normal);
    OUT.texCoord = IN[1].texCoord;
    EmitVertex();
	
    gl_Position = explode(gl_in[2].gl_Position, normal);
    OUT.texCoord = IN[2].texCoord;
    EmitVertex();
	
}
