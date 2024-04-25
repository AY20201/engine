#version 440 core
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gLightPosition;
layout (location = 2) out vec3 gNormal;
layout (location = 3) out vec4 gAlbedo;
layout (location = 4) out vec3 gViewSpacePosition;

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

void main()
{
	vec3 norm = normalize(normal);
	
	vec3 normalMapSample = vec3(0.0, 0.0, 0.0);

	if(triplanar == 1)
	{
		vec3 blending = GetTriplanarBlend(norm);
		normalMapSample = GetTriplanarColor(normalMap, currentPos, blending, normalMapScale); //0.5
		gAlbedo = vec4(GetTriplanarColor(albedo, currentPos, blending, albedoScale), 1.0) * vec4(albedoColor, 1.0);
	}
	else
	{
		normalMapSample = texture(normalMap, texCoord * normalMapScale).rgb;
		gAlbedo = texture(albedo, texCoord * albedoScale) * vec4(albedoColor, 1.0);
	}

	//vec3 normalMapSample = (texture(normalMap, texCoord * normalMapScale).rgb * 2.0 - 1.0) * normalMapStrength;
	//vec3 normalMapSample = GetTriplanarColor(normalMap, currentPos, blending, normalMapScale);
	normalMapSample = (normalMapSample * 2.0 - 1.0) * normalMapStrength;
	vec3 tan = normalize(tangent);
	tan = normalize(tan - dot(tan, norm) * norm);
	vec3 bitangent = cross(tan, norm);
	mat3 TBN = mat3(tan, bitangent, norm);
	norm = normalize(TBN * normalMapSample);

	gPosition = vec4(currentPos, gl_FragCoord.z);
	gLightPosition = currentPosLightSpace;
	gNormal = norm;

	//gAlbedo = texture(albedo, texCoord * albedoScale).rgb * albedoColor;
	//gAlbedo = GetTriplanarColor(albedo, currentPos, blending, albedoScale) * albedoColor;
	gViewSpacePosition = viewSpacePosition;
}