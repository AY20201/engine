#include"../engine_headers/Light.h"
#include"../engine_headers/LightHandler.h"

class SpotLight : public Light
{
public:

	glm::vec3 position;
	glm::vec3 direction;

	float innerCutoffAngle;
	float outerCutoffAngle;

	SpotLight(glm::vec3 position, glm::vec3 direction, float innerCutoffAngle, float outerCutoffAngle, glm::vec3 color, float intensity)
	{
		SpotLight::position = position;
		SpotLight::direction = direction;
		SpotLight::innerCutoffAngle = innerCutoffAngle;
		SpotLight::outerCutoffAngle = outerCutoffAngle;

		Light::color = color;
		Light::intensity = intensity;
		Light::type = Light::LightType::Spot;

		index = static_cast<int>(LightHandler::Instance.spotLights.size());
		LightHandler::Instance.AddLight(this);
	}

	void SetUniforms(Shader& shader) override
	{
		shader.Activate();
		glUniform3f(glGetUniformLocation(shader.ID, ("spotLights[" + std::to_string(index) + "].position").c_str()), position.x, position.y, position.z);
		glUniform3f(glGetUniformLocation(shader.ID, ("spotLights[" + std::to_string(index) + "].direction").c_str()), direction.x, direction.y, direction.z);
		glUniform3f(glGetUniformLocation(shader.ID, ("spotLights[" + std::to_string(index) + "].color").c_str()), color.x, color.y, color.z);
		glUniform1f(glGetUniformLocation(shader.ID, ("spotLights[" + std::to_string(index) + "].intensity").c_str()), intensity);

		glUniform1f(glGetUniformLocation(shader.ID, ("spotLights[" + std::to_string(index) + "].innerCutoffAngle").c_str()), glm::cos(glm::radians(innerCutoffAngle)));
		glUniform1f(glGetUniformLocation(shader.ID, ("spotLights[" + std::to_string(index) + "].outerCutoffAngle").c_str()), glm::cos(glm::radians(outerCutoffAngle)));
	}
};