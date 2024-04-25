#include"../engine_headers/Material.h"

Material::Material(Shader& shader, TexParam albedo, TexParam normalMap, bool transparent/*, TexParam specMap*/)
{
	Material::shader = shader;
	
	Material::albedo = albedo;
	Material::normalMap = normalMap;

	Material::transparent = transparent;
	//Material::specMap = specMap;
}

void Material::SetTextures()
{
	shader.Activate();

	std::vector<TexParam> textures{ albedo, normalMap/*, specMap*/};

	for (int i = 0; i < textures.size(); i++)
	{
		if (!textures[i].Empty())
		{
			SetTexture(textures[i].tex, textures[i].uniformName);

			shader.Activate();
			std::string scaleUniName = std::string(textures[i].uniformName) + std::string("Scale");
			glUniform1f(glGetUniformLocation(shader.ID, scaleUniName.c_str()), textures[i].scale);
		}
	}

	shader.Activate();
	glUniform3f(glGetUniformLocation(shader.ID, "albedoColor"), albedo.color.x, albedo.color.y, albedo.color.z);
}

void Material::SetTexture(Texture* tex, const char* uniName)
{
	if (tex != nullptr)
	{
		tex->SetUniform(shader, uniName);
		tex->Bind();
	}
}

void Material::Clear()
{
	delete albedo.tex;
	delete normalMap.tex;
}