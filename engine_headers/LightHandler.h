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

		void AddLight(Light* light);
		void SetLightUniforms(Shader& shader);

};

#endif
