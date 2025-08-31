#version 430 core
out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D renderedScene;
uniform sampler2D renderedSceneDepth;
uniform sampler2D bloomBlur;

uniform float bloomStrength;

//float bloomStrength = 0.08;//0.08;
float exposure = 1.4;//1.5;
float gamma = 0.7;

void main()
{
	vec3 hdrCol = texture(renderedScene, texCoord).rgb;
	vec3 bloomCol = texture(bloomBlur, texCoord).rgb;
	vec3 col = mix(hdrCol, bloomCol, bloomStrength);

	vec3 mapped = vec3(1.0) - exp(-col * exposure);
	mapped = pow(mapped, vec3(1.0 / gamma));

	gl_FragDepth = texture(renderedSceneDepth, texCoord).r;

	FragColor = vec4(mapped, 1.0);
}