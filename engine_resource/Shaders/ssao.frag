#version 430 core
out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D gNormal;
uniform sampler2D gViewSpacePosition;
uniform sampler2D noiseTexture;

uniform vec3 samples[16];
uniform mat4 projection;
uniform mat4 view;

uniform vec2 noiseScale;

float nearPlane = 0.1;
float farPlane = 100.0;

float radius = 0.125;

/*
float CalculateViewZ(vec2 tCoord)
{
	float depth = texture(gDepth, tCoord).x;
	float viewZ = projection[3][2] / (2.0 * depth - 1.0 - projection[2][2]);
	return viewZ;
}
*/

void main()
{
	vec3 currentPos = texture(gViewSpacePosition, texCoord).xyz;

	vec3 normal = mat3(transpose(inverse(view))) * texture(gNormal, texCoord).rgb;
	vec3 randomVec = texture(noiseTexture, texCoord * noiseScale).xyz;

	vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN = mat3(tangent, bitangent, normal);

	float occlusion = 0.0;
	for(int i = 0; i < 16; i++)
	{
		vec3 samplePos = TBN * samples[i];
		samplePos = currentPos + samplePos * radius;

		vec4 offset = vec4(samplePos, 1.0);
		offset = projection * offset;
		offset.xy /= offset.w;
		offset.xy = offset.xy * 0.5 + 0.5;

		
		//float ndc = texture(gPosition, offset.xy).a;
		//float sampleDepth = ndc >= 1.0 ? farPlane : texture(gViewSpacePosition, offset.xy).z;
		float sampleDepth = texture(gViewSpacePosition, offset.xy).z;

		float rangeCheck = smoothstep(0.0, 1.0, radius / abs(currentPos.z - sampleDepth));
		occlusion += (sampleDepth >= samplePos.z + 0.025 ? 1.0 : 0.0) * rangeCheck;
	}

	occlusion = 1.0 - (occlusion / 16);

	FragColor = vec4(occlusion, occlusion, occlusion, 1.0);
}