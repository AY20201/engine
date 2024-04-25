#ifndef MATERIAL_CLASS_H
#define MATERIAL_CLASS_H

#include<glm/glm.hpp>
#include"Texture.h"
#include"Shader.h"

#include<vector>

struct TexParam
{
	const char* uniformName = "";
	Texture* tex = nullptr;
	float scale = 1.0f;
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

	bool Empty()
	{
		return uniformName == "" || tex == nullptr;
	}
};

class Material
{
	public:

		Shader shader = Shader();
		
		TexParam albedo;
		TexParam normalMap;
		//TexParam specMap;
		bool transparent = false;

		Material(Shader& shader, TexParam albedo, TexParam normalMap, bool transparent/*, TexParam specMap*/);
		Material() = default;
		void SetTextures();
		void Clear();

	private:

		void SetTexture(Texture* tex, const char* uniName);
};

#endif
