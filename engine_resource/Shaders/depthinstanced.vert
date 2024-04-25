#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in mat4 instanceMatrix;

out vec2 texCoord;

uniform mat4 lightMatrix;
uniform mat4 modelMatrix;

void main()
{
	texCoord = aTex;
	gl_Position = lightMatrix * instanceMatrix * vec4(aPos, 1.0);
}