#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include<glad/glad.h>
#include<stb/stb_image.h>
#include<vector>

#include"Shader.h"

class Texture
{
	public:
		static GLuint activeTexUnit;

		static Texture* defaultNormalMap;
		static Texture* defaultAlbedo;
		//static Texture* defaultSpecMap;
		static std::vector<Texture*> loadedTextures;

		GLuint ID;
		GLenum type;
		GLuint texUnit;
		const char* filepath;
		bool set = false;
		bool useMipmaps = true;

		Texture() = default;
		Texture(const char* imageFile, GLenum textureType, GLenum filter, GLuint slot, GLenum format, GLenum pixelType, bool useMipmaps = true);
		Texture(const char* imageFile, GLenum textureType, GLenum filter, GLenum format, GLenum pixelType, bool useMipmaps = true);
		//static Texture* GetTexture(const char* imageFile, GLenum textureType, GLenum filter, GLuint slot, GLenum format, GLenum pixelType);
		//static Texture* GetTexture(const char* imageFile, GLenum textureType, GLenum filter, GLenum format, GLenum pixelType);
		void SetUniform(Shader& shader, const char* uniformName);
		void Bind();
		void Unbind();
		void Delete();
		//static Texture* CreateTextureAttachment(int width, int height, GLuint texUnit);
		//static Texture* CreateDepthTextureAttachment(int width, int height, GLuint texUnit);
		//static Texture* SearchLoadedTextures(const char* imageFile);
		//static void ClearAll();
};

#endif