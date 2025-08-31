#ifndef LIGHTHANDLER_CLASS_H
#define LIGHTHANDLER_CLASS_H

#include "Light.h"
#include "Shader.h"
#include <vector>

class LightHandler
{
	public:

		static LightHandler Instance;
		std::vector <Light*> pointLights;
		std::vector <Light*> dirLights;
		std::vector <Light*> spotLights;

		void AddLight(Light* light);
		void SetLightUniforms(Shader& shader);
		void SetSingleLightUniforms(Shader& shader, int index);

};

#endif
