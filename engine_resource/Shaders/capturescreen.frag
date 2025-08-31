#version 430 core
out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D renderedScene;
uniform int empty;
uniform float time;

float rand(vec2 pos){
	return fract(sin(dot(pos.xy, vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
	if(empty == 0) {
		vec3 col = vec3(0.0, 0.0, 0.0);

		float grainScale = 0.0002;
		vec2 roundedPos = vec2(floor(texCoord.x / grainScale) * grainScale, floor(texCoord.y / grainScale) * grainScale);
		float grain = rand(roundedPos * time) * 0.4 - 0.025;
		col.rgb += grain;

		FragColor = vec4(col, 1.0);
	} else {
		vec3 col = texture(renderedScene, texCoord).rgb;
		FragColor = vec4(col, 1.0);
	}
}