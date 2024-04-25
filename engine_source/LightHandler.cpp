#include"../engine_headers/LightHandler.h"

LightHandler LightHandler::Instance;

void LightHandler::AddLight(Light* light)
{
	if (light->type == Light::LightType::Point)
	{
		pointLights.push_back(light);
	}
	
	if (light->type == Light::LightType::Directional)
	{
		dirLights.push_back(light);
	}
	
}

void LightHandler::SetLightUniforms(Shader& shader)
{
	shader.Activate();

	glUniform1i(glGetUniformLocation(shader.ID, "numPointLights"), static_cast<int>(pointLights.size()));
	glUniform1i(glGetUniformLocation(shader.ID, "numDirLights"), static_cast<int>(dirLights.size()));

	for (int i = 0; i < static_cast<int>(pointLights.size()); i++)
	{
		pointLights[i]->SetUniforms(shader);
	}

	for (int i = 0; i < static_cast<int>(dirLights.size()); i++)
	{
		dirLights[i]->SetUniforms(shader);
	}
}