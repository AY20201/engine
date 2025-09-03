#ifndef CAMERAQUAD_CLASS_H
#define CAMERAQUAD_CLASS_H

#include"Mesh.h"
#include"FrameBufferObject.h"
#include"Camera.h"
#include"Texture.h"
#include"Light.h"
#include<algorithm>

class CameraQuad {
	public:
		glm::mat4 transformMatrix;
		Mesh quadMesh;
		float quadScale = 0.15f;//0.12f;

		VAO secondQuadVAO;
		std::vector<Vertex> secondQuadVerts;

		float targetFov = 65.0f;
		float currentFov = 65.0f;
		//float captureDelay = 2.0f;
		bool captureKeyPressed = false;
		bool disabled = false;
		bool fullscreen = false;
		bool maxFullscreen = false;
		bool captureDelayFrame = false;

		glm::vec3 localCameraOrientation;
		glm::vec3 mainCameraForward;
		glm::vec3 mainCameraRight;
		glm::vec3 mainCameraUp;
		
		glm::vec3 quadPos;
		glm::vec3 globalCameraOrientation;

		std::vector<TextureObject> screenCaptures;
		TextureObject emptyTexObject;
		float lastCaptureTime = 0.0f;
		int activeCapture = -1;

		Light* flashLight;
		float flashTime = 0.1f;
		float defaultExposure = 0.02f;

		CameraQuad(float width, float height, glm::vec3 initialOrientation, Light* flashLight);
		void RenderMainQuad(TextureObject& cameraRender, Shader& shader);
		void RenderSecondQuad(TextureObject& captureImage, Shader& shader);
		void UpdateMatrix(glm::vec3 mainCameraPos, float offset);
		void RotateCamera(GLFWwindow* window, float deltaTime, float currentTime);
		void ToggleDisabled(GLFWwindow* window);
		void GetCameraVectors(glm::vec3 cameraOrientation);
		void AdjustFOV(float scrollValue);
		void AdjustExposure(GLFWwindow* window);

		void CaptureScreen(TextureObject& captureTexture, int screenWidth, int screenHeight, GLFWwindow* window, bool useFlash);
		TextureObject& GetLastCapture();
		void UpdateActiveCapture(GLFWwindow* window);
		TextureObject& GetActiveCapture();
		void ClearCapture(int index);
		void CheckFlash(float deltaTime);
	private:
		glm::vec3 orientationNoVerticalRotation;
		bool targetNeutralRotation = false;
		bool disabledKeyPressed = false;
		bool fullscreenKeyPressed = false;
		bool arrowKeyPressed = false;
		bool exposureKeyPressed = false;
		float flashStarted;
		float currentTime;
		//float capturePressTime;
};

#endif