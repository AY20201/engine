#version 430 core
out vec4 FragColor;

in vec2 texCoord;
in vec3 normal;
in vec3 currentPos;
in vec4 currentPosLightSpace;
in vec3 tangent;

struct PointLight {
	vec3 position;
	vec3 color;
	float intensity;

	float initAtten;
	float constAtten;
	float linearAtten;
	float expAtten;
};

struct DirectionalLight {
	vec3 direction;
	vec3 color;
	float intensity;
};

#define PI 3.14159265

#define MAX_POINT_LIGHTS 10
uniform int numPointLights;
uniform PointLight pointLights[MAX_POINT_LIGHTS];

#define MAX_DIR_LIGHTS 1
uniform int numDirLights;
uniform DirectionalLight dirLights[MAX_DIR_LIGHTS];

uniform sampler2D albedo;
uniform float albedoScale;
uniform vec3 albedoColor;
uniform sampler2D normalMap;
uniform float normalMapScale;
uniform vec3 camPos;

uniform sampler2D cameraImage;
uniform sampler2D screenUVMap;

uniform sampler2D lastCapture;
uniform float lastCaptureTime;
uniform float currentTime;

uniform samplerCube skybox;

uniform sampler2D shadowMap;
uniform sampler2D jitterMap;

float displayCaptureDelay = 0.0;

float ambientFactor = 1.0;
float specularStrength = 0.5;
float specPower = 8;
float normalMapStrength = 0.5;

//float initAtten = 1.0;
//float constantAtten = 1.0;
//float linearAtten = 0.5;
//float expAtten = 0.3;

float brightnessThreshold = 0.05; //unused value, corresponds to 7.2
float distanceThreshold = 7.2;

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 specFactor)
{
	vec3 pos = light.position;
	vec3 color = light.color;
	float inten = light.intensity;
	
	vec3 norm = normalize(normal);
	vec3 lightDirection = normalize(pos - fragPos);
	float diffuse = max(dot(norm, lightDirection), 0.0);
	
	vec3 reflectionDirection = reflect(-lightDirection, norm);
	float specAmount = pow(max(dot(viewDir, reflectionDirection), 0.0), specPower);
	vec3 specular = specAmount * specFactor;
	
	vec3 eyeDir = normalize(fragPos - camPos);
	vec3 environmentReflectDir = reflect(eyeDir, norm);
	vec3 skyboxSample = vec3(texture(skybox, environmentReflectDir));
	float skyboxBrightness = 0.2126 * skyboxSample.r + 0.7152 * skyboxSample.g + 0.0722 * skyboxSample.b;

	float dist = distance(pos, fragPos);
	
	float falloff = light.initAtten / (light.constAtten + light.linearAtten * dist + light.expAtten * dist * dist);

	return (diffuse + specular * skyboxSample) * inten * falloff * color;
}

vec3 CalculateDirLight(DirectionalLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 specFactor, float sceneAmbience, float shadow)
{
	vec3 dir = light.direction;
	vec3 color = light.color;
	float inten = light.intensity;

	vec3 norm = normalize(normal);
	vec3 lightDirection = normalize(-dir);
	float diffuse = max(dot(norm, lightDirection), 0.0);

	vec3 reflectionDirection = reflect(-lightDirection, norm);
	float specAmount = pow(max(dot(viewDir, reflectionDirection), 0.0), specPower);
	vec3 specular = specAmount * specFactor;

	vec3 eyeDir = normalize(fragPos - camPos);
	vec3 environmentReflectDir = reflect(eyeDir, norm);
	vec3 skyboxSample = vec3(texture(skybox, environmentReflectDir));
	float skyboxBrightness = 0.2126 * skyboxSample.r + 0.7152 * skyboxSample.g + 0.0722 * skyboxSample.b;

	return ((diffuse + specular * skyboxSample) * (1.0 - shadow)) * inten * sceneAmbience * color;
}

float CalculatePixelLum(vec4 sampleColor)
{
	vec3 adjustedLum = vec3(0.2126, 0.7152, 0.0722);
	return (sampleColor.r * adjustedLum.r + sampleColor.g * adjustedLum.g + sampleColor.b * adjustedLum.b);
}

void main()
{
	vec3 norm = normalize(normal);

	vec3 normalMapSample = (texture(normalMap, texCoord * normalMapScale).rgb * 2.0 - 1.0) * normalMapStrength;
	vec3 tan = normalize(tangent);
	tan = normalize(tan - dot(tan, norm) * norm);
	vec3 bitangent = cross(tan, norm);
	mat3 TBN = mat3(tan, bitangent, norm);
	norm = normalize(TBN * normalMapSample);

	vec3 specularSample = vec3(specularStrength, specularStrength, specularStrength);//texture(specMap, texCoord * specMapScale).rgb;

	vec3 viewDir = normalize(camPos - currentPos);
	
	vec3 adjustedLuminace = vec3(0.2126, 0.7152, 0.0722);
	//int numMipMaps = textureQueryLevels(skybox);

	vec4 topSkySample = textureLod(skybox, vec3(0.0, 1.0, 0.0), 10.0);
	vec4 bottomSkySample = textureLod(skybox, vec3(0.0, -1.0, 0.0), 10.0);
	vec4 rightSkySample = textureLod(skybox, vec3(1.0, 0.0, 0.0), 10.0);
	vec4 leftSkySample = textureLod(skybox, vec3(-1.0, 0.0, 0.0), 10.0);
	vec4 frontSkySample = textureLod(skybox, vec3(0.0, 0.0, 1.0), 10.0);
	vec4 backSkySample = textureLod(skybox, vec3(0.0, 0.0, 1.0), 10.0);

	float avgSceneLum = (CalculatePixelLum(topSkySample) + 
						CalculatePixelLum(bottomSkySample) + 
						CalculatePixelLum(rightSkySample) + 
						CalculatePixelLum(leftSkySample) + 
						CalculatePixelLum(frontSkySample) +
						CalculatePixelLum(backSkySample)) / 6.0;
	
	float sceneAmbience = avgSceneLum * ambientFactor;
	
	vec3 lightResult = vec3(0, 0, 0);
	float shadowBias = 0.0002;
	float shadow = 0.0;

	for(int i = 0; i < numDirLights; i++) {
		lightResult += CalculateDirLight(dirLights[i], normal, currentPos, viewDir, specularSample, avgSceneLum, shadow);
	}
		
	for(int i = 0; i < numPointLights; i++) {
		lightResult += CalculatePointLight(pointLights[i], normal, currentPos, viewDir, specularSample);
	}
	
	if(currentTime - displayCaptureDelay < lastCaptureTime){
		FragColor = texture(lastCapture, texCoord * albedoScale);
	} else {
		vec2 screenUVCoords = texture(screenUVMap, texCoord * albedoScale).xy;
		//screenUVCoords = vec2(screenUVCoords.x, 1.0 - screenUVCoords.y);
		vec4 screen = texture(cameraImage, texCoord * albedoScale);
		//FragColor = vec4(screenUVCoords.x, screenUVCoords.y, 0.0, 1.0);
		FragColor = screen + texture(albedo, screenUVCoords) * vec4(lightResult, 1.0);
	}
}