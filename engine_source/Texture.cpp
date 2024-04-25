#include"../engine_headers/Texture.h"

GLuint Texture::activeTexUnit = 0;
Texture* Texture::defaultAlbedo;
Texture* Texture::defaultNormalMap;
//Texture* Texture::defaultSpecMap;
std::vector<Texture*> Texture::loadedTextures;

Texture::Texture(const char* imageFile, GLenum textureType, GLenum filter, GLuint slot, GLenum format, GLenum pixelType, bool useMipmaps)
{
	filepath = imageFile;
	type = textureType;
	set = true;
	Texture::useMipmaps = useMipmaps;

	int imgWidth, imgHeight, numColChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* bytes = stbi_load(imageFile, &imgWidth, &imgHeight, &numColChannels, 0);

	glGenTextures(1, &ID);
	texUnit = slot;
	Bind();

	glTexImage2D(type, 0, GL_RGBA, imgWidth, imgHeight, 0, format, pixelType, bytes);

	glGenerateMipmap(type);
	glTexParameteri(type, GL_TEXTURE_MIN_FILTER, useMipmaps ? GL_LINEAR_MIPMAP_LINEAR : filter);
	glTexParameteri(type, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_REPEAT);

	stbi_image_free(bytes);
	Unbind();
}

Texture::Texture(const char* imageFile, GLenum textureType, GLenum filter, GLenum format, GLenum pixelType, bool useMipmaps)
{
	filepath = imageFile;
	type = textureType;
	set = true;
	Texture::useMipmaps = useMipmaps;

	int imgWidth, imgHeight, numColChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* bytes = stbi_load(imageFile, &imgWidth, &imgHeight, &numColChannels, 0);

	glGenTextures(1, &ID);
	texUnit = Texture::activeTexUnit;
	Texture::activeTexUnit += 1;
	Bind();

	if (numColChannels == 3)
	{
		format = GL_RGB;
	}
	else if (numColChannels == 4)
	{
		format = GL_RGBA;
	}

	glTexImage2D(type, 0, GL_RGBA, imgWidth, imgHeight, 0, format, pixelType, bytes);
	
	glGenerateMipmap(type);
	glTexParameteri(type, GL_TEXTURE_MIN_FILTER, useMipmaps ? GL_LINEAR_MIPMAP_LINEAR : filter);
	glTexParameteri(type, GL_TEXTURE_MAG_FILTER, filter);
	//glTexParameteri(type, GL_TEXTURE_MAX_LEVEL, 9);
	//glTexParameteri(type, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	stbi_image_free(bytes);
	Unbind();
}
/*
Texture* Texture::GetTexture(const char* imageFile, GLenum textureType, GLenum filter, GLuint slot, GLenum format, GLenum pixelType)
{
	Texture* existingTex = Texture::SearchLoadedTextures(imageFile);

	if (existingTex != nullptr)
	{
		return existingTex;
	}

	Texture* newTex = new Texture(imageFile, textureType, filter, slot, format, pixelType);
	return newTex;
}

Texture* Texture::GetTexture(const char* imageFile, GLenum textureType, GLenum filter, GLenum format, GLenum pixelType)
{
	Texture* existingTex = Texture::SearchLoadedTextures(imageFile);

	if (existingTex != nullptr)
	{
		return existingTex;
	}

	delete existingTex;

	Texture* newTex = new Texture(imageFile, textureType, filter, format, pixelType);
	return newTex;
}
*/
void Texture::SetUniform(Shader& shader, const char* uniformName)
{
	GLuint texID = glGetUniformLocation(shader.ID, uniformName);
	shader.Activate();
	glUniform1i(texID, texUnit);

	//std::string scaleUniName = std::string(uniformName) + std::string("Scale");

	//glUniform1f(glGetUniformLocation(shader.ID, scaleUniName.c_str()), scale);
}

void Texture::Bind()
{
	glActiveTexture(GL_TEXTURE0 + texUnit);
	glBindTexture(type, ID);
}

void Texture::Unbind()
{
	glBindTexture(type, 0);
}

void Texture::Delete()
{
	glDeleteTextures(1, &ID);
}
/*
Texture* Texture::CreateTextureAttachment(int width, int height, GLuint texUnit) {
	Texture* texture = new Texture();
	texture->texUnit = texUnit;

	texture->Bind();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture->ID, 0);
	texture->Unbind();

	return texture;
}

Texture* Texture::CreateDepthTextureAttachment(int width, int height, GLuint texUnit) {
	Texture* texture = new Texture();
	texture->texUnit = texUnit;

	texture->Bind();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture->ID, 0);
	texture->Unbind();

	return texture;
}

Texture* Texture::SearchLoadedTextures(const char* imageFile)
{
	Texture* foundTex = nullptr;

	for (unsigned int i = 0; i < Texture::loadedTextures.size(); i++)
	{
		if (std::strcmp(imageFile, Texture::loadedTextures[i]->filepath) == 0)
		{
			foundTex = Texture::loadedTextures[i];
		}
	}

	return foundTex;
}

void Texture::ClearAll()
{
	for (unsigned int i = 0; i < Texture::loadedTextures.size(); i++)
	{
		delete Texture::loadedTextures[i];
	}
}
*/