#version 430 core
layout (location = 0) out vec3 Upsample;

in vec2 texCoord;

uniform sampler2D sourceTexture;
uniform float filterRadius;

void main()
{
	float x = filterRadius;
    float y = filterRadius;

	vec3 a = texture(sourceTexture, vec2(texCoord.x - x, texCoord.y + y)).rgb;
    vec3 b = texture(sourceTexture, vec2(texCoord.x,     texCoord.y + y)).rgb;
    vec3 c = texture(sourceTexture, vec2(texCoord.x + x, texCoord.y + y)).rgb;

    vec3 d = texture(sourceTexture, vec2(texCoord.x - x, texCoord.y)).rgb;
    vec3 e = texture(sourceTexture, vec2(texCoord.x,     texCoord.y)).rgb;
    vec3 f = texture(sourceTexture, vec2(texCoord.x + x, texCoord.y)).rgb;

    vec3 g = texture(sourceTexture, vec2(texCoord.x - x, texCoord.y - y)).rgb;
    vec3 h = texture(sourceTexture, vec2(texCoord.x,     texCoord.y - y)).rgb;
    vec3 i = texture(sourceTexture, vec2(texCoord.x + x, texCoord.y - y)).rgb;

    Upsample = e * 4.0;
    Upsample += (b + d + f + h) * 2.0;
    Upsample += (a + c + g + i);
    Upsample *= 1.0 / 16.0;
}
