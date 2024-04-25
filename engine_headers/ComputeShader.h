#ifndef COMPUTESHADER_CLASS_H
#define COMPUTESHADER_CLASS_H

#include"Texture.h"
#include"Shader.h"

class ComputeShader
{
public:
	GLuint ID;
	GLuint textureID;
	GLuint textureUnit;

	int attachedTextureWidth;
	int attachedTextureHeight;

	ComputeShader(const char* file);
	ComputeShader() = default;

	void AttachTexture(int textureWidth, int textureHeight);
	void BindTexture();
	void SetTexture(Shader& shader, const char* uniformName);
	void Dispatch();
	void Activate();
	void Delete();
};

#endif
