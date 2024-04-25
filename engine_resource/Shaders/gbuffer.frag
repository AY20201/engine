#version 440 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedo;

in vec3 fragPos;
in vec2 texCoord;
in vec3 normal;

void main()
{
	gPosition = fragPos;
	gNormal = normalize(normal);
	gAlbedo.rgb = vec3(0.95, 0.95, 0.95);
}