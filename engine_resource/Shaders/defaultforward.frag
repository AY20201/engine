#version 440 core
out vec4 FragColor;

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

uniform vec3 camPos;

uniform samplerCube skybox;

uniform sampler2D shadowMap;
uniform sampler2D jitterMap;

float ambientFactor = 1.25;
float specularStrength = 0.5;
float specPower = 8;

//float initAtten = 1.0;
//float constantAtten = 1.0;
//float linearAtten = 0.5;
//float expAtten = 0.3;

float brightnessThreshold = 0.05; //unused value, corresponds to 7.2
float distanceThreshold = 7.2;

in vec2 texCoord;
in vec3 normal;
in vec3 currentPos;
in vec4 currentPosLightSpace;
in vec3 tangent;
in vec3 viewSpacePosition;

uniform sampler2D albedo;
uniform float albedoScale;
uniform vec3 albedoColor;
uniform sampler2D normalMap;
uniform float normalMapScale;

uniform int triplanar;

float normalMapStrength = 0.5;

vec3 GetTriplanarBlend(vec3 norm)
{
	vec3 blending = abs(norm);
	blending = normalize(max(blending, 0.00001));
	float b = (blending.x + blending.y + blending.z);
	blending /= vec3(b, b, b);
	return blending;
}

vec3 GetTriplanarColor(sampler2D tex, vec3 pos, vec3 blending, float scale)
{
	vec3 xAxis = texture(tex, pos.yz * scale).rgb;
	vec3 yAxis = texture(tex, pos.xz * scale).rgb;
	vec3 zAxis = texture(tex, pos.xy * scale).rgb;

	return xAxis * blending.x + yAxis * blending.y + zAxis * blending.z;
}

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 specFactor, float sceneAmbience, float shadow)
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

	return (diffuse * (1.0 - shadow)) * inten * sceneAmbience * color;
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
	
	vec3 normalMapSample = vec3(0.0, 0.0, 0.0);
	vec4 albedoCol = vec4(0.0);

	if(triplanar == 1)
	{
		vec3 blending = GetTriplanarBlend(norm);
		normalMapSample = GetTriplanarColor(normalMap, currentPos, blending, normalMapScale);
		albedoCol = vec4(GetTriplanarColor(albedo, currentPos, blending, 1.0), 1.0) * vec4(albedoColor, 1.0);
	}
	else
	{
		normalMapSample = texture(normalMap, texCoord * normalMapScale).rgb;
		albedoCol = texture(albedo, texCoord * albedoScale) * vec4(albedoColor, 1.0);
	}
	/*
	normalMapSample = (normalMapSample * 2.0 - 1.0) * normalMapStrength;
	vec3 tan = normalize(tangent);
	tan = normalize(tan - dot(tan, norm) * norm);
	vec3 bitangent = cross(tan, norm);
	mat3 TBN = mat3(tan, bitangent, norm);
	norm = normalize(TBN * normalMapSample);
	*/
	float ambientOcculsion = 1.0;//texture(ssao, texCoord).r;

	vec3 specularSample = vec3(specularStrength, specularStrength, specularStrength);

	vec3 viewDir = normalize(camPos - currentPos);

	vec3 adjustedLuminace = vec3(0.2126, 0.7152, 0.0722);
	
	vec4 topSkySample = textureLod(skybox, vec3(0.0, 1.0, 0.0), 10.0);
	vec4 bottomSkySample = textureLod(skybox, vec3(0.0, -1.0, 0.0), 10.0);
	//vec4 rightSkySample = textureLod(skybox, vec3(1.0, 0.0, 0.0), 10.0);
	//vec4 leftSkySample = textureLod(skybox, vec3(-1.0, 0.0, 0.0), 10.0);
	//vec4 frontSkySample = textureLod(skybox, vec3(0.0, 0.0, 1.0), 10.0);
	//vec4 backSkySample = textureLod(skybox, vec3(0.0, 0.0, 1.0), 10.0);
	/*
	float avgSceneLum = (CalculatePixelLum(topSkySample) + 
						CalculatePixelLum(bottomSkySample) + 
						CalculatePixelLum(rightSkySample) + 
						CalculatePixelLum(leftSkySample) + 
						CalculatePixelLum(frontSkySample) +
						CalculatePixelLum(backSkySample)) / 6.0;
	*/
	float avgSceneLum = (CalculatePixelLum(topSkySample) + 
						CalculatePixelLum(bottomSkySample)) / 2.0;
	
	float sceneAmbience = avgSceneLum * ambientFactor;

	vec3 lightResult = vec3(0, 0, 0);
	float shadow = 0.0;

	lightResult = CalculateDirLight(dirLights[0], normal, currentPos, viewDir, specularSample, avgSceneLum, 0.0);

	if(albedoCol.a == 0.0)
	{
		discard; //not exactly pretty but necessary because otherwise I would need to sort each individual leaf fragment
	}
	FragColor = albedoCol * vec4(max(lightResult, sceneAmbience), 1.0) * ambientOcculsion;
}