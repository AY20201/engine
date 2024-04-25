#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;

uniform float aspectRatio;
uniform float tanHalfFov;

out vec2 texCoord;
out vec2 viewRay;

void main()
{
	gl_Position = vec4(aPos.x, aPos.y, 0.0f, 1.0f);
	texCoord = aTex;
	viewRay.x = aPos.x * aspectRatio * tanHalfFov;
	viewRay.y = aPos.y * tanHalfFov;
}
