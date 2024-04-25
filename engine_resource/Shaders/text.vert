#version 430 core
layout (location = 0) in vec4 vertex; //pos, texcoord

out vec2 texCoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
	gl_Position = projection * view * model * vec4(vertex.xy, 0.0, 1.0);
	texCoord = vertex.zw;
}