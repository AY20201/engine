#ifndef FRAMEBUFFEROBJECT_CLASS_H
#define FRAMEBUFFEROBJECT_CLASS_H

#include<glad/glad.h>
#include<stb/stb_image.h>

#include"Texture.h"
#include"Mesh.h"

const Vertex quadVertices[] =
{
	Vertex{glm::vec3(-1.0f, -1.0f,  0.0f), glm::vec2(0.0f, 0.0f)},
	Vertex{glm::vec3(1.0f, 1.0f,  0.0f), glm::vec2(1.0f, 1.0f)},
	Vertex{glm::vec3(-1.0f, 1.0f,  0.0f), glm::vec2(0.0f, 1.0f)},
	
	Vertex{glm::vec3(-1.0f, -1.0f,  0.0f), glm::vec2(0.0f, 0.0f)},
	Vertex{glm::vec3(1.0f, -1.0f,  0.0f), glm::vec2(1.0f, 0.0f)},
	Vertex{glm::vec3(1.0f, 1.0f,  0.0f), glm::vec2(1.0f, 1.0f)}
};

struct TextureObject
{
	GLuint textureID;
	GLuint textureUnit;
};

class FrameBufferObject
{
public:
	FrameBufferObject() = default;
	FrameBufferObject(int screenWidth, int screenHeight, int numColorTex, int numDepthTex);
	FrameBufferObject(int screenWidth, int screenHeight);
	
	int screenWidth, screenHeight;

	VAO fullscreenQuadVAO;
	std::vector<Vertex> quadVerts;

	GLuint bufferID;
	std::vector<TextureObject> depthTextures;
	std::vector<TextureObject> colorTextures;
	//TextureObject depthTexture;
	//TextureObject colorTexture;

	void AttachDepthTexture(TextureObject& texObj);
	void AttachColorTexture(TextureObject& texObj, GLuint index, GLenum colorSpace, GLenum pixelType);
	void SetUpGBuffer();
	void BindTexture(TextureObject& texObj);
	void UnbindTexture();
	void BindFrameBuffer();
	void UnbindFrameBuffer();
	void SetTexture(TextureObject& texObj, Shader& shader, const char* uniformName);
	
	void InitializeRenderQuad();
	void RenderQuad(Shader& shader);

	void Delete();
};

#endif
