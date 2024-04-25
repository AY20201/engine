#version 430 core
out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D renderedScene;
uniform sampler2D renderedSceneDepth;

float nearPlane = 0.1;
float farPlane = 200.0;

float maxDist = 50.0;//50.0;
float minDist = 1.0;
vec4 fogColor = vec4(0.39, 0.4, 0.4, 1.0);

float density = 0.00;//0.035;
float minFog = 0.0;//0.25;

void main()
{
	float depthSample = texture(renderedSceneDepth, texCoord).r;
	float depth = (2.0 * nearPlane * farPlane) / (farPlane + nearPlane - (2.0 * depthSample - 1.0) * (farPlane - nearPlane));
	//float fogFactor = clamp((maxDist - depth) / (maxDist - minDist), 0.0, 1.0);
	float fogFactor = 1.0 / pow(2, (depth * density) * (depth * density));

	vec4 sceneColor = texture(renderedScene, texCoord);
	vec4 outputColor = mix(fogColor, sceneColor, clamp(fogFactor, minFog, 1.0));

	gl_FragDepth = min(depthSample, 0.9999); //make sure output doesn't cover skybox
	FragColor = outputColor;
}