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
uniform sampler2D specMap;
uniform float specMapScale;
uniform vec3 camPos;

uniform samplerCube skybox;

uniform sampler2D shadowMap;
uniform sampler2D jitterMap;

float ambientFactor = 1.0;
float specularStrength = 0.5;
float specPower = 8;
float normalMapStrength = 0.5;

float initAtten = 1.0;
float constantAtten = 1.0;
float linearAtten = 0.5;
float expAtten = 0.3;

float brightnessThreshold = 0.05; //unused value, corresponds to 7.2
float distanceThreshold = 7.2;

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 specFactor, float sceneAmbience, float shadow)
{
	vec3 pos = light.position;
	vec3 color = light.color;
	float iten = light.intensity;

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
	//float falloff = 1.0 / pow(distance(pos, fragPos), 2);
	float falloff = initAtten / (constantAtten + linearAtten * dist + expAtten * dist * dist);

	return (diffuse + specular * skyboxSample) * iten * falloff * color;
}

vec3 CalculateDirLight(DirectionalLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 specFactor, float sceneAmbience, float shadow)
{
	vec3 dir = light.direction;
	vec3 color = light.color;
	float iten = light.intensity;

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

	return ((diffuse + specular * skyboxSample) * (1.0 - shadow)) * iten * sceneAmbience * color;
}

float CalculatePixelLum(vec4 sampleColor)
{
	vec3 adjustedLum = vec3(0.2126, 0.7152, 0.0722);
	return (sampleColor.r * adjustedLum.r + sampleColor.g * adjustedLum.g + sampleColor.b * adjustedLum.b);
}

float CalculateShadow(vec4 fragPosLightSpace, float bias)
{
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;

	if(projCoords.z > 1.0)
	{
		return 0.0;
	}

	float currentDepth = projCoords.z;

	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

	float softness = 1.25;
	float jitterSize = 1.5;

	int sampleRadius = 2;

	for(int x = -sampleRadius; x <= sampleRadius; x++)
	{
		for(int y = -sampleRadius; y <= sampleRadius; y++)
		{
			vec2 scaled = vec2(x, y) / float(sampleRadius);
			vec2 circle = vec2(scaled.x * sqrt(1.0 - 0.5 * (scaled.y * scaled.y)), scaled.y * sqrt(1.0 - 0.5 * (scaled.x * scaled.x)));

			vec2 jitterSample = texture(jitterMap, projCoords.xy + vec2(x, y) * softness * texelSize).xy;
			float pcfDepth = texture(shadowMap, projCoords.xy + (circle + jitterSample * jitterSize) * softness * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}
	
	shadow /= ((sampleRadius * 2.0 + 1.0) * (sampleRadius * 2.0 + 1.0));

	return shadow;
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
		shadow = CalculateShadow(currentPosLightSpace, shadowBias);
		lightResult += CalculateDirLight(dirLights[i], norm, currentPos, viewDir, specularSample, avgSceneLum, shadow);
	}
		
	for(int i = 0; i < numPointLights; i++) {
		lightResult += CalculatePointLight(pointLights[i], norm, currentPos, viewDir, specularSample, avgSceneLum, shadow);
	}
	
	//lightResult += CalculatePointLight(vec3(-0.8, 1.0, -0.5), norm, currentPos, viewDir);
	
	//float4 diffuseColor = texture(albedo, texCoord * albedoScale);

	FragColor = texture(albedo, texCoord * albedoScale) * vec4(albedoColor, 1.0) * vec4(max(lightResult, sceneAmbience), 1.0);
	//FragColor = vec4(1.5, 1.5, 1.5, 1.0);
}