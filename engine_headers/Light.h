#ifndef LIGHT_CLASS_H
#define LIGHT_CLASS_H

#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>

#include"Shader.h"

class Light
{
	public:

		enum class LightType { Directional, Point };
		LightType type = LightType::Point;
		int index = 0;

		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
		float intensity = 1.0f;

		Light() = default;
		virtual void SetUniforms(Shader& shader)
		{
			
		};

};

#endif
