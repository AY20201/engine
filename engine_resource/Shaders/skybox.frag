#version 430 core
out vec4 FragColor;

in vec3 texCoord;

uniform samplerCube skybox;

void main()
{
	gl_FragDepth = 1.0;
	FragColor = texture(skybox, texCoord);
}