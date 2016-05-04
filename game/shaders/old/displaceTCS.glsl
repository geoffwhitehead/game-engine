#version 400 core

layout (vertices=4) out; // num vertices in output patch

in Vertex {
	vec2 texCoord; // from vertex shader
} IN[];				// equal to size of the draw call vertex count

out Vertex {
	vec2 texCoord;	//to evaluation shader

} OUT[];			// equal to the size of the layout vertex count

void main(void)	{
	
	gl_TessLevelInner[0] = 128;
	gl_TessLevelInner[1] = 128;
	
	gl_TessLevelOuter[0] = 128;
	gl_TessLevelOuter[1] = 128;
	gl_TessLevelOuter[2] = 128;
	gl_TessLevelOuter[3] = 128;
	
	OUT[gl_InvocationID].texCoord = IN[gl_InvocationID].texCoord;
	
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	
}