#version 430 core
out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D renderedScene;
uniform sampler2D renderedSceneDepth;

void main()
{
	vec3 col = texture(renderedScene, texCoord).rgb;

	gl_FragDepth = texture(renderedSceneDepth, texCoord).r;
	FragColor = vec4(col, 1.0);
}