#version 430 core
out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D renderedScene;
uniform sampler2D renderedSceneDepth;
uniform sampler2D gPosition;

uniform float time;
uniform mat4 camMatrix;
uniform mat4 prevViewProjMatrix;

float width = 3840.0 / 8;
float height = 2160.0 / 8;

vec2 blockSize = vec2(0.02160 / 14, 0.03840 / 14);
int motionBlurSamples = 5;
float maxBlurSize = 0.05 * 0.05;

float rand(vec2 pos){
	return fract(sin(dot(pos.xy, vec2(12.9898,78.233))) * 43758.5453);
}

float interlace(vec2 uv) {
    return step(0.5, mod(floor(uv.y * (2160.0 / 14.0)) + floor(time * 60.0), 2.0));
}

void main()
{
	vec3 col = texture(renderedScene, texCoord).rgb;
	float depth = texture(renderedSceneDepth, texCoord).r;

	vec3 compressed = texture(renderedScene, floor(texCoord / blockSize) * blockSize).rgb;

	//col = compressed;//mix(col, compressed, 0.9);

	//https://developer.nvidia.com/gpugems/gpugems3/part-iv-image-effects/chapter-27-motion-blur-post-processing-effect
	/*
	vec4 worldPos = texture(gPosition, texCoord);
	vec4 currentPos = camMatrix * worldPos;
	vec4 previousPos = prevViewProjMatrix * worldPos;
	previousPos /= previousPos.w;
	currentPos /= currentPos.w;
	vec2 velocity = -(currentPos - previousPos).xy * 0.10;
	
	if(length(velocity) > 0.001){
		if(length(velocity) > maxBlurSize){
			velocity *= (maxBlurSize / length(velocity));
		}
		//velocity = clamp(velocity, -maxBlurSize, maxBlurSize);
		if(depth == 0.9999){
			velocity = vec2(0.0);
		}
		vec2 tempTexCoord = texCoord + velocity;

		for(int i = 1; i < motionBlurSamples; i++, tempTexCoord += velocity){
			//float sampleDepth = texture(renderedSceneDepth, tempTexCoord).r;
			col += texture(renderedScene, floor(tempTexCoord / blockSize) * blockSize).rgb;
		}
		col /= motionBlurSamples;
	}
	*/
	float n = rand(floor(texCoord / blockSize) * blockSize * time) * 0.1;
	col.r += n;
	col.b -= n;

	float intensity = dot(col, vec3(0.299, 0.587, 0.114));
    col = mix(vec3(intensity), col, 1.2);

	vec2 offset = vec2(1.0) / vec2(3840 / 14, 2160 / 14) * 0.6;
    //vec3 edge = texture2D(renderedScene, floor((texCoord + offset) / blockSize) * blockSize).rgb - col;
	vec3 edge = texture2D(renderedScene, texCoord).rgb - col;
    col += edge * 0.2;

	col *= mix(1.0, 0.92, interlace(texCoord));
	
	//https://www.shadertoy.com/view/lsKSWR
	vec2 vigUV = texCoord;
	vigUV *= 1.0 - vigUV;
	float vig = vigUV.x * vigUV.y * 15.0;
	vig = pow(vig, 0.07);
	col *= vig;

	gl_FragDepth = depth;
	FragColor = vec4(col, 1.0);
}