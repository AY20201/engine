#include"../engine_headers/FrameBufferObject.h"

FrameBufferObject::FrameBufferObject(int screenWidth, int screenHeight, int numColorTex, int numDepthTex) {
	FrameBufferObject::screenWidth = screenWidth;
	FrameBufferObject::screenHeight = screenHeight;

	glGenFramebuffers(1, &bufferID);

	for (int i = 0; i < numDepthTex; i++)
	{
		TextureObject depthTexObj = TextureObject{};
		depthTextures.push_back(depthTexObj);
		AttachDepthTexture(depthTextures[i]);
	}
	
	for (int i = 0; i < numColorTex; i++)
	{
		TextureObject colTexObj = TextureObject{};
		colorTextures.push_back(colTexObj);
		AttachColorTexture(colorTextures[i], i, GL_RGBA16F, GL_FLOAT);
	}

	if (numColorTex == 0)
	{
		unsigned int attachments[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, attachments);
	}
	else if(numColorTex == 1)
	{
		unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, attachments);
	}
	else if (numColorTex == 2)
	{
		unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, attachments);
	}
	

	UnbindFrameBuffer();
	UnbindTexture();
}

FrameBufferObject::FrameBufferObject(int screenWidth, int screenHeight) {
	FrameBufferObject::screenWidth = screenWidth;
	FrameBufferObject::screenHeight = screenHeight;

	glGenFramebuffers(1, &bufferID);

	TextureObject depthTexObj = TextureObject{};
	depthTextures.push_back(depthTexObj);
	AttachDepthTexture(depthTextures[0]);

	UnbindFrameBuffer();
	UnbindTexture();
}

void FrameBufferObject::AttachDepthTexture(TextureObject& texObj)
{
	texObj.textureUnit = Texture::activeTexUnit;
	Texture::activeTexUnit++;

	glGenTextures(1, &texObj.textureID);
	glBindTexture(GL_TEXTURE_2D, texObj.textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, screenWidth, screenHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	UnbindTexture();

	glBindFramebuffer(GL_FRAMEBUFFER, bufferID);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texObj.textureID, 0);
	//glDrawBuffer(GL_NONE);
	//glReadBuffer(GL_NONE);
}

void FrameBufferObject::AttachColorTexture(TextureObject& texObj, GLuint index, GLenum colorSpace, GLenum pixelType)
{
	texObj.textureUnit = Texture::activeTexUnit;
	Texture::activeTexUnit++;

	glGenTextures(1, &texObj.textureID);
	glBindTexture(GL_TEXTURE_2D, texObj.textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, colorSpace, screenWidth, screenHeight, 0, GL_RGBA, pixelType, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	UnbindTexture();

	glBindFramebuffer(GL_FRAMEBUFFER, bufferID);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, texObj.textureID, 0);
	//glDrawBuffer(GL_NONE);
	//glReadBuffer(GL_NONE);
}

void FrameBufferObject::SetUpGBuffer()
{
	TextureObject colTexObj = TextureObject{};
	colorTextures.push_back(colTexObj);

	AttachColorTexture(colorTextures[0], 0, GL_RGBA32F, GL_FLOAT);

	TextureObject colTexObj1 = TextureObject{};
	colorTextures.push_back(colTexObj1);

	AttachColorTexture(colorTextures[1], 1, GL_RGBA32F, GL_FLOAT);
	
	TextureObject colTexObj2 = TextureObject{};
	colorTextures.push_back(colTexObj2);

	AttachColorTexture(colorTextures[2], 2, GL_RGBA16F, GL_FLOAT);

	TextureObject colTexObj3 = TextureObject{};
	colorTextures.push_back(colTexObj3);

	AttachColorTexture(colorTextures[3], 3, GL_RGBA16F, GL_FLOAT);

	TextureObject colTexObj4 = TextureObject{};
	colorTextures.push_back(colTexObj4);

	AttachColorTexture(colorTextures[4], 4, GL_RGBA16F, GL_FLOAT);

	unsigned int attachments[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
	glDrawBuffers(5, attachments);

	UnbindFrameBuffer();
	UnbindTexture();
}

void FrameBufferObject::Delete() {
	glDeleteFramebuffers(1, &bufferID);

	for (unsigned int i = 0; i < depthTextures.size(); i++)
	{
		glDeleteTextures(1, &depthTextures[i].textureID);
	}

	for (unsigned int i = 0; i < colorTextures.size(); i++)
	{
		glDeleteTextures(1, &colorTextures[i].textureID);
	}
}

void FrameBufferObject::BindTexture(TextureObject& texObj)
{
	glActiveTexture(GL_TEXTURE0 + texObj.textureUnit);
	glBindTexture(GL_TEXTURE_2D, texObj.textureID);
}

void FrameBufferObject::UnbindTexture()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void FrameBufferObject::UnbindFrameBuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, screenWidth, screenHeight);
}

void FrameBufferObject::BindFrameBuffer() {
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, bufferID);
	glViewport(0, 0, screenWidth, screenHeight);
}

void FrameBufferObject::SetTexture(TextureObject& texObj, Shader& shader, const char* uniformName)
{
	shader.Activate();
	glUniform1i(glGetUniformLocation(shader.ID, uniformName), texObj.textureUnit);
	BindTexture(texObj);
}

void FrameBufferObject::InitializeRenderQuad()
{
	quadVerts = std::vector<Vertex>(quadVertices, quadVertices + sizeof(quadVertices) / sizeof(Vertex));

	fullscreenQuadVAO.Bind();
	VBO vbo(quadVerts);

	fullscreenQuadVAO.LinkAttrib(vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
	fullscreenQuadVAO.LinkAttrib(vbo, 1, 2, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
	fullscreenQuadVAO.LinkAttrib(vbo, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)(5 * sizeof(float)));
	fullscreenQuadVAO.LinkAttrib(vbo, 3, 3, GL_FLOAT, sizeof(Vertex), (void*)(8 * sizeof(float)));

	fullscreenQuadVAO.Unbind();
	vbo.Unbind();
}

void FrameBufferObject::RenderQuad(Shader& shader)
{
	shader.Activate();

	fullscreenQuadVAO.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

/*
GLuint FrameBufferObject::CreateDepthTextureAttachment() {
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, screenWidth, screenHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture, 0);

	return texture;
}
*/

/*
GLuint FrameBufferObject::CreateDepthBufferAttachment() {
	GLuint depthBuffer;
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenWidth, screenHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

	return depthBuffer;
}
*/