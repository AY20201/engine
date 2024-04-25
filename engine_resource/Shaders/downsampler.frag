#version 430 core
layout (location = 0) out vec3 Downsample;

in vec2 texCoord;

uniform sampler2D sourceTexture;
uniform vec2 sourceResolution;

void main()
{
	vec2 texelSize = 1.0 / sourceResolution;
	float x = texelSize.x;
	float y = texelSize.y;

	vec3 a = texture(sourceTexture, vec2(texCoord.x - 2 * x, texCoord.y + 2 * y)).rgb;
    vec3 b = texture(sourceTexture, vec2(texCoord.x,       texCoord.y + 2 * y)).rgb;
    vec3 c = texture(sourceTexture, vec2(texCoord.x + 2 * x, texCoord.y + 2 * y)).rgb;

    vec3 d = texture(sourceTexture, vec2(texCoord.x - 2 * x, texCoord.y)).rgb;
    vec3 e = texture(sourceTexture, vec2(texCoord.x,       texCoord.y)).rgb;
    vec3 f = texture(sourceTexture, vec2(texCoord.x + 2 * x, texCoord.y)).rgb;

    vec3 g = texture(sourceTexture, vec2(texCoord.x - 2 * x, texCoord.y - 2 * y)).rgb;
    vec3 h = texture(sourceTexture, vec2(texCoord.x,       texCoord.y - 2 * y)).rgb;
    vec3 i = texture(sourceTexture, vec2(texCoord.x + 2 * x, texCoord.y - 2 * y)).rgb;

    vec3 j = texture(sourceTexture, vec2(texCoord.x - x, texCoord.y + y)).rgb;
    vec3 k = texture(sourceTexture, vec2(texCoord.x + x, texCoord.y + y)).rgb;
    vec3 l = texture(sourceTexture, vec2(texCoord.x - x, texCoord.y - y)).rgb;
    vec3 m = texture(sourceTexture, vec2(texCoord.x + x, texCoord.y - y)).rgb;

    Downsample = e * 0.125;
    Downsample += (a + c + g + i) * 0.03125;
    Downsample += (b + d + f + h) * 0.0625;
    Downsample += (j + k + l + m) * 0.125;

    Downsample = max(Downsample, 0.0001);
}
