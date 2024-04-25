#version 430 core
out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D renderedScene;

void main()
{
	vec2 texelSize = 1.0 / vec2(textureSize(renderedScene, 0));
    float result = 0.0;
    for (int x = -2; x < 2; x++) 
    {
        for (int y = -2; y < 2; y++) 
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(renderedScene, texCoord + offset).r;
        }
    }
    float blur = result / (4.0 * 4.0);

    FragColor = vec4(blur, blur, blur, 1.0);
}