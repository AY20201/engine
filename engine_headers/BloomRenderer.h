#ifndef BLOOMRENDERER_CLASS_H
#define BLOOMRENDERER_CLASS_H

#include"Shader.h"
#include"FrameBufferObject.h"
#include"Texture.h"

struct MipTex
{
	glm::vec2 res;
	glm::ivec2 intRes;
	GLuint textureID;
};

class BloomRenderer
{
public:
	GLuint fboID;
	GLuint mipTexUnit;
	FrameBufferObject fboObj;

	std::vector<MipTex> mipChain;
	BloomRenderer(int windowWidth, int windowHeight, int mipChainLength);
	void InitializeMipChain();
	void RenderBloomTexture(Shader& upsampleShader, Shader& downsampleShader, GLuint sourceTexture, GLuint sourceTextureUnit, float filterRadius);
	void Delete();

	GLuint BloomTexture();
	void BindFBO();
	void UnbindFBO();

private:
	int windowWidth;
	int windowHeight;

	int mipChainLength;
	void RenderDownsamples(Shader& downsampleShader, GLuint sourceTexture, GLuint sourceTextureUnit);
	void RenderUpsamples(Shader& upsampleShader, GLuint sourceTextureUnit, float filterRadius);
};

#endif
