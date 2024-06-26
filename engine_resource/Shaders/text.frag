#version 430 core
out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D text;
uniform vec3 textColor;

float transparency = 0.8;

void main()
{
	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, texCoord).r * transparency);
	FragColor = vec4(textColor, 1.0) * sampled;
}