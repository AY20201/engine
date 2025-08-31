#version 430 core
out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D renderedScene;
uniform sampler2D renderedSceneDepth;
uniform float time;
uniform int isCapturing;

float contrastFactor = 0.85;
vec3 warmthBiasRGB = vec3(1.2, 1.05, 0.95);
vec3 chromaticAberrationOffsets = vec3(0.009, 0.006, -0.006);
float fadeAmount = 0.2;
float grainAmount = 0.15;
float grainScale = 0.0002;
float speckScale = 0.0008;

vec3 filmicToneMap(vec3 color) {
    color = max(vec3(0.0), color - 0.004);
    vec3 result = (color * (6.2 * color + 0.5)) / (color * (6.2 * color + 1.7) + 0.06);
	return pow(result, vec3(2.7));
}

float rand(vec2 pos){
	return fract(sin(dot(pos.xy, vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
	vec3 col = vec3(0.0);//texture(renderedScene, texCoord).rgb;

	vec2 chrAberrationDir = (texCoord - vec2(0.5, 0.5)) * 1.2;
	col.r = texture(renderedScene, texCoord + chrAberrationDir * vec2(chromaticAberrationOffsets.r)).r;
	col.g = texture(renderedScene, texCoord + chrAberrationDir * vec2(chromaticAberrationOffsets.g)).g;
	col.b = texture(renderedScene, texCoord + chrAberrationDir * vec2(chromaticAberrationOffsets.b)).b;

	col *= warmthBiasRGB; //bias red colors over blue

	col.rgb = mix(col.rgb, vec3(dot(col.rgb, vec3(0.33))), fadeAmount); //fade colors by blending with grayscale

	col.rgb = filmicToneMap(col.rgb); //filmic tonemapping

	float luminance = 0.2126f * col.r + 0.7152f * col.g + 0.0722f * col.b;

	vec2 roundedPos = vec2(floor(texCoord.x / grainScale) * grainScale, floor(texCoord.y / grainScale) * grainScale);
	float grain = rand(roundedPos * time) * grainAmount - 0.025;
	col.rgb += grain;

	//adding specks on screen
	vec2 roundedSpeckPos = floor(gl_FragCoord.xy / 7.0);//vec2(floor(texCoord.x / speckScale) * speckScale, floor(texCoord.y / speckScale) * speckScale);

	float opacityRand = rand(roundedSpeckPos);
	float xStrech = rand(roundedSpeckPos + 1.83);
	float yStrech = rand(roundedSpeckPos + 4.32);
	float rotation = rand(roundedSpeckPos + 8.17);
	
	vec2 speckFrequencyOpacity = isCapturing == 1 ? vec2(0.01, 0.5) : vec2(0.05, 0.8);
	float speckChance = speckFrequencyOpacity.x;
	float speck = step(1.0 - speckChance, rand(roundedSpeckPos * time));
	vec2 local = fract(gl_FragCoord.xy / 7.0) - 0.5;

	float angle = rotation * 6.2831;
	mat2 rot = mat2(cos(angle), -sin(angle), sin(angle), cos(angle));

	vec2 shaped = rot * local;
	vec2 scaled = shaped * vec2(mix(0.3, 1.7, xStrech), mix(0.3, 1.7, yStrech));

	float dist = length(scaled);
	float opacity = mix(0.1, speckFrequencyOpacity.y, opacityRand);
	speck *= smoothstep(0.4, 0.1, dist) * opacity * (luminance + 0.4); // Rounded blob
	col = mix(col, vec3(0.05), speck);

	//vignette
	vec2 vigUV = texCoord;
	vigUV *= 1.0 - vigUV;
	float vig = vigUV.x * vigUV.y * 15.0;
	vig = pow(vig, 0.07);
	col *= vig;

	gl_FragDepth = texture(renderedSceneDepth, texCoord).r;
	FragColor = vec4(col, 1.0);

	//FragColor = vec4(vec3(luminance), 1.0);
}