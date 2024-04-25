#include"../engine_headers/Light.h"
#include"../engine_headers/LightHandler.h"

class PointLight : public Light
{
	public:

		glm::vec3 position;

		float initAtten;
		float constAtten;
		float linearAtten;
		float expAtten;

		PointLight(glm::vec3 position, glm::vec3 color, float intensity, float initAtten = 1.0f, float constAtten = 1.0f, float linearAtten = 0.5f, float expAtten = 0.3f)
		{
			PointLight::position = position;

			PointLight::initAtten = initAtten;
			PointLight::constAtten = constAtten;
			PointLight::linearAtten = linearAtten;
			PointLight::expAtten = expAtten;

			Light::color = color;
			Light::intensity = intensity;
			Light::type = Light::LightType::Point;

			index = static_cast<int>(LightHandler::Instance.pointLights.size());
			LightHandler::Instance.AddLight(this);
		}

		void SetUniforms(Shader& shader) override
		{
			shader.Activate();
			glUniform3f(glGetUniformLocation(shader.ID, ("pointLights[" + std::to_string(index) + "].position").c_str()), position.x, position.y, position.z);
			glUniform3f(glGetUniformLocation(shader.ID, ("pointLights[" + std::to_string(index) + "].color").c_str()), color.x, color.y, color.z);
			glUniform1f(glGetUniformLocation(shader.ID, ("pointLights[" + std::to_string(index) + "].intensity").c_str()), intensity);

			glUniform1f(glGetUniformLocation(shader.ID, ("pointLights[" + std::to_string(index) + "].initAtten").c_str()), initAtten);
			glUniform1f(glGetUniformLocation(shader.ID, ("pointLights[" + std::to_string(index) + "].constAtten").c_str()), constAtten);
			glUniform1f(glGetUniformLocation(shader.ID, ("pointLights[" + std::to_string(index) + "].linearAtten").c_str()), linearAtten);
			glUniform1f(glGetUniformLocation(shader.ID, ("pointLights[" + std::to_string(index) + "].expAtten").c_str()), expAtten);
		}
};