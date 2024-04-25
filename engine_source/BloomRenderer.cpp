#include"../engine_headers/BloomRenderer.h"

BloomRenderer::BloomRenderer(int windowWidth, int windowHeight, int mipChainLength)
{
	BloomRenderer::windowWidth = windowWidth;
	BloomRenderer::windowHeight = windowHeight;
	BloomRenderer::mipChainLength = mipChainLength;
	BloomRenderer::mipTexUnit = Texture::activeTexUnit;
	Texture::activeTexUnit += 1;

	glGenFramebuffers(1, &fboID);
	BindFBO();

	InitializeMipChain();

	UnbindFBO();

	fboObj.InitializeRenderQuad();
}

void BloomRenderer::InitializeMipChain()
{
	glm::vec2 mipSize(windowWidth, windowHeight);
	glm::ivec2 mipSizeInt(windowWidth, windowHeight);

	for (int i = 0; i < mipChainLength; i++)
	{
		MipTex mip;

		mipSize *= 0.5f;
		mipSizeInt /= 2;
		mip.res = mipSize;
		mip.intRes = mipSizeInt;

		glGenTextures(1, &mip.textureID);
		glBindTexture(GL_TEXTURE_2D, mip.textureID);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_R11F_G11F_B10F, mipSizeInt.x, mipSizeInt.y, 0, GL_RGB, GL_FLOAT, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		mipChain.emplace_back(mip);
	}

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mipChain[0].textureID, 0);
	GLuint attachments[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, attachments);
}

GLuint BloomRenderer::BloomTexture()
{
	return mipChain[0].textureID;
}

void BloomRenderer::BindFBO()
{
	glBindFramebuffer(GL_FRAMEBUFFER, fboID);
}

void BloomRenderer::UnbindFBO()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void BloomRenderer::Delete()
{
	for (int i = 0; i < mipChain.size(); i++) {
		glDeleteTextures(1, &mipChain[i].textureID);
		mipChain[i].textureID = 0;
	}
	glDeleteFramebuffers(1, &fboID);
	fboID = 0;
}

void BloomRenderer::RenderBloomTexture(Shader& upsampleShader, Shader& downsampleShader, GLuint sourceTexture, GLuint sourceTextureUnit, float filterRadius)
{
	upsampleShader.Activate();
	glUniform1i(glGetUniformLocation(upsampleShader.ID, "sourceTexture"), sourceTextureUnit);
	downsampleShader.Activate();
	glUniform1i(glGetUniformLocation(downsampleShader.ID, "sourceTexture"), sourceTextureUnit);

	BindFBO();

	RenderDownsamples(downsampleShader, sourceTexture, sourceTextureUnit);
	RenderUpsamples(upsampleShader, sourceTextureUnit, filterRadius);

	UnbindFBO();

	glViewport(0, 0, windowWidth, windowHeight);
}

void BloomRenderer::RenderDownsamples(Shader& downsampleShader, GLuint sourceTexture, GLuint sourceTextureUnit)
{
	downsampleShader.Activate();
	glUniform2f(glGetUniformLocation(downsampleShader.ID, "sourceResolution"), (float)windowWidth, (float)windowHeight);

	glActiveTexture(GL_TEXTURE0 + sourceTextureUnit);
	glBindTexture(GL_TEXTURE_2D, sourceTexture);

	for (int i = 0; i < mipChain.size(); i++)
	{
		MipTex& mip = mipChain[i];
		glViewport(0, 0, mip.intRes.x, mip.intRes.y);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mip.textureID, 0);

		fboObj.RenderQuad(downsampleShader);

		glUniform2f(glGetUniformLocation(downsampleShader.ID, "sourceResolution"), mip.res.x, mip.res.y);
		glBindTexture(GL_TEXTURE_2D, mip.textureID);
	}

	glUseProgram(0);
}

void BloomRenderer::RenderUpsamples(Shader& upsampleShader, GLuint sourceTextureUnit, float filterRadius)
{
	upsampleShader.Activate();
	glUniform1f(glGetUniformLocation(upsampleShader.ID, "filterRadius"), filterRadius);

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glBlendEquation(GL_FUNC_ADD);

	for (int i = static_cast<int>(mipChain.size()) - 1; i > 0; i--)
	{
		MipTex& mip = mipChain[i];
		MipTex& nextMip = mipChain[i - 1];

		glActiveTexture(GL_TEXTURE0 + sourceTextureUnit);
		glBindTexture(GL_TEXTURE_2D, mip.textureID);

		glViewport(0, 0, nextMip.intRes.x, nextMip.intRes.y);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, nextMip.textureID, 0);

		fboObj.RenderQuad(upsampleShader);
	}

	glDisable(GL_BLEND);

	glUseProgram(0);
}