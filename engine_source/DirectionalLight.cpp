#include"../engine_headers/Light.h"
#include"../engine_headers/LightHandler.h"

class DirectionalLight : public Light
{
	public:

		glm::vec3 direction;
		DirectionalLight(glm::vec3 direction, glm::vec3 color, float intensity)
		{
			DirectionalLight::direction = direction;
			Light::color = color;
			Light::intensity = intensity;
			Light::type = Light::LightType::Directional;

			index = static_cast<int>(LightHandler::Instance.dirLights.size());
			LightHandler::Instance.AddLight(this);
		}

		void SetUniforms(Shader& shader) override
		{
			shader.Activate();
			glUniform3f(glGetUniformLocation(shader.ID, ("dirLights[" + std::to_string(index) + "].direction").c_str()), direction.x, direction.y, direction.z);
			glUniform3f(glGetUniformLocation(shader.ID, ("dirLights[" + std::to_string(index) + "].color").c_str()), color.x, color.y, color.z);
			glUniform1f(glGetUniformLocation(shader.ID, ("dirLights[" + std::to_string(index) + "].intensity").c_str()), intensity);
		}
};