#include <iostream>

#include<Windows.h>
#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include"engine_headers/Shader.h"
#include"engine_headers/ComputeShader.h"
#include"engine_headers/FrameBufferObject.h"
#include"engine_headers/Mesh.h"
#include"engine_headers/CollisionMesh.h"
#include"engine_headers/Texture.h"
#include"engine_headers/Material.h"
#include"engine_headers/Camera.h"
#include"engine_headers/GameObject.h"
#include"engine_headers/ObjectHandler.h"
#include"engine_headers/LightHandler.h"
#include"engine_source/PointLight.cpp"
#include"engine_source/DirectionalLight.cpp"
#include"engine_source/SpotLight.cpp"
#include"engine_headers/Skybox.h"
#include"engine_headers/MeshScene.h"
#include"engine_headers/BloomRenderer.h"
#include"engine_headers/ShadowChunker.h"
#include"engine_headers/CameraQuad.h"

extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 1;
	_declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

float nearClipPlane = 0.1f;
float farClipPlane = 500.0f;

int main()
{
	glfwInit();

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	//specify opengl version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//specify opengl profile, core has most up to date functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

	const int width = mode->width;
	const int height = mode->height;

	//create window
	GLFWwindow* window = glfwCreateWindow(width, height, "Project1", monitor, NULL);
	if (window == NULL)
	{
		std::cout << "Window failed to create" << std::endl;
		glfwTerminate();
		return -1;
	}

	//tell glfw to use window in current context
	glfwMakeContextCurrent(window);

	//load opengl through glad
	gladLoadGL();

	glViewport(0, 0, width, height);

	ComputeShader jitterComputeShader("../../engine_resource/Shaders/jitter.comp");
	jitterComputeShader.AttachTexture(2048, 2048);

	Texture* defaultAlbedo = new Texture("../../engine_resource/Textures/default_albedo.png", GL_TEXTURE_2D, GL_LINEAR, /*1,*/ GL_RGB, GL_UNSIGNED_BYTE);
	Texture* defaultNormalMap = new Texture("../../engine_resource/Textures/default_normal.png", GL_TEXTURE_2D, GL_LINEAR, /*1,*/ GL_RGB, GL_UNSIGNED_BYTE);
	Texture* defaultTransparent = new Texture("../../engine_resource/Textures/default_transparent.png", GL_TEXTURE_2D, GL_LINEAR, /*1,*/ GL_RGBA, GL_UNSIGNED_BYTE);
	Texture* cameraScreenUVMap = new Texture("../../engine_resource/Textures/camera/screen_uvmap.png", GL_TEXTURE_2D, GL_LINEAR, /*1,*/ GL_RGBA, GL_UNSIGNED_BYTE);
	//Texture* blackTexture = new Texture("../../engine_resource/Textures/black_texture.png", GL_TEXTURE_2D, GL_LINEAR, /*1,*/ GL_RGBA, GL_UNSIGNED_BYTE);

	//TextureObject blackTextureObj = TextureObject{ blackTexture->ID, blackTexture->texUnit };

	Texture::defaultAlbedo = defaultAlbedo;
	Texture::defaultNormalMap = defaultNormalMap;

	int cam2ResolutionRatio = 8;
	FrameBufferObject lightingFrameBuffer(width, height, 1, 1);
	FrameBufferObject lightingFrameBuffer2(width / cam2ResolutionRatio, height / cam2ResolutionRatio, 1, 1);
	FrameBufferObject fogFrameBuffer(width, height, 1, 1, true);
	FrameBufferObject fogFrameBuffer2(width / cam2ResolutionRatio, height / cam2ResolutionRatio, 1, 1, true);
	FrameBufferObject toneMapFrameBuffer(width, height, 1, 1);
	FrameBufferObject toneMapFrameBuffer2(width / cam2ResolutionRatio, height / cam2ResolutionRatio, 1, 1);
	FrameBufferObject filterFrameBuffer(width, height, 1, 1);
	
	FrameBufferObject shadowMapFrameBuffer(2048, 2048, 1, 1);
	FrameBufferObject basePostFrameBuffer(width, height);
	FrameBufferObject basePostFrameBuffer2(width / cam2ResolutionRatio, height / cam2ResolutionRatio);
	basePostFrameBuffer.SetUpGBuffer();
	basePostFrameBuffer2.SetUpGBuffer();
	basePostFrameBuffer.InitializeRenderQuad();

	Shader shaderProgram("../../engine_resource/Shaders/default.vert", "../../engine_resource/Shaders/defaultdeferred.frag");
	Shader shadowShaderProgram("../../engine_resource/Shaders/depth.vert", "../../engine_resource/Shaders/depth.frag");
	Shader skyBoxShaderProgram("../../engine_resource/Shaders/skybox.vert", "../../engine_resource/Shaders/skybox.frag");
	Shader basePostShaderProgram("../../engine_resource/Shaders/postprocess.vert", "../../engine_resource/Shaders/basepostprocesser.frag");
	Shader lightingShaderProgram("../../engine_resource/Shaders/postprocess.vert", "../../engine_resource/Shaders/lighting.frag");
	Shader tonemapperShaderProgram("../../engine_resource/Shaders/postprocess.vert", "../../engine_resource/Shaders/tonemapper.frag");
	Shader bloomDownsampleShaderProgram("../../engine_resource/Shaders/postprocess.vert", "../../engine_resource/Shaders/downsampler.frag");
	Shader bloomUpsampleShaderProgram("../../engine_resource/Shaders/postprocess.vert", "../../engine_resource/Shaders/upsampler.frag");
	Shader fogShaderProgram("../../engine_resource/Shaders/postprocess.vert", "../../engine_resource/Shaders/fog.frag");
	Shader filmShaderProgram("../../engine_resource/Shaders/postprocess.vert", "../../engine_resource/Shaders/film.frag");
	Shader camcorderShaderProgram("../../engine_resource/Shaders/postprocess.vert", "../../engine_resource/Shaders/camcorder.frag");
	Shader blackAndWhiteShaderProgram("../../engine_resource/Shaders/postprocess.vert", "../../engine_resource/Shaders/blackandwhite.frag");
	Shader screenShaderProgram("../../engine_resource/Shaders/default.vert", "../../engine_resource/Shaders/screen.frag");
	Shader captureScreenShaderProgram("../../engine_resource/Shaders/postprocess.vert", "../../engine_resource/Shaders/capturescreen.frag");

	BloomRenderer bloomRenderer(width, height, 7);
	BloomRenderer bloomRenderer2(width / cam2ResolutionRatio, height / cam2ResolutionRatio, 7);
	
	ShadowChunker shadowChunker(0.5f);

	Skybox sceneSkyBox(std::vector<const char*>{
		"../../engine_resource/Textures/cloud_skybox/clouds1_east.bmp", //right
		"../../engine_resource/Textures/cloud_skybox/clouds1_west.bmp", //left
		"../../engine_resource/Textures/cloud_skybox/clouds1_up.bmp", //up
		"../../engine_resource/Textures/cloud_skybox/clouds1_down.bmp", //down
		"../../engine_resource/Textures/cloud_skybox/clouds1_north.bmp", //front
		"../../engine_resource/Textures/cloud_skybox/clouds1_south.bmp"}, //back
		skyBoxShaderProgram
	);
	
	DirectionalLight globalDirectionalLight(glm::vec3(0.6f, -0.5f, -0.5f), glm::vec3(1.0f, 0.965f, 0.89f), 2.5f);
	PointLight light(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);
	SpotLight flashLight(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 20.0f, 50.0f, glm::vec3(1.0f, 1.0f, 1.0f), 0.0f);
	//PointLight light1(glm::vec3(-2.0f, 0.8f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);

	CameraQuad cameraQuad(3.840f, 2.160f, glm::vec3(0.0f, 0.0f, -1.0f), &flashLight);
	glfwSetWindowUserPointer(window, &cameraQuad);

	Camera camera(width, height, glm::vec3(0.0f, 1.0f, 3.0f), true);
	Camera camera2(width, height, glm::vec3(0.0f, 1.0f, 5.0f), true);

	ObjectHandler::currentSceneIndex = 0;
	ObjectHandler::scenes.push_back(ObjectHandler());

	MeshScene monkey(Transform::Zero, std::vector<Behavior*>{ nullptr }, std::vector<const char*>{ "../../engine_resource/3D Objects/monkey/monkey.obj" }, shaderProgram, nullptr, false, false, false);
	MeshScene floor(Transform::Zero, std::vector<Behavior*>{ nullptr }, std::vector<const char*>{ "../../engine_resource/3D Objects/tower/floor/floor.obj" }, shaderProgram, nullptr, false, false, false);
	MeshScene floor1(Transform::Zero, std::vector<Behavior*>{ nullptr }, std::vector<const char*>{ "../../engine_resource/3D Objects/tower/floor1/floor1.obj" }, shaderProgram, nullptr, false, false, false);
	MeshScene cameraModel(Transform::Zero, std::vector<Behavior*>{ nullptr }, std::vector<const char*>{ "../../engine_resource/3D Objects/camera/camera.obj" }, shaderProgram, nullptr, false, false, false, false);
	//disable screen visibility
	cameraModel.sceneGameObjects[0]->meshes[7].visible = false;
	cameraModel.sceneGameObjects[0]->meshes[7].material->shader = screenShaderProgram;
	//MeshScene room(Transform::Zero, std::vector<Behavior*>{ nullptr }, std::vector<const char*>{ "../../engine_resource/3D Objects/room/room.obj" }, shaderProgram, nullptr, false, false, false);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	double currentTime = glfwGetTime();
	double previousTime = glfwGetTime();
	float deltaTime = 0.0f;
	Shader postProcessingChoice = filmShaderProgram;

	glm::mat4 prevCamMatrix = glm::mat4(1.0f);

	LightHandler::Instance.SetLightUniforms(lightingShaderProgram);
	LightHandler::Instance.SetLightUniforms(screenShaderProgram);

	ObjectHandler::scenes[ObjectHandler::currentSceneIndex].Awake();

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		currentTime = glfwGetTime();
		deltaTime = (float)currentTime - (float)previousTime;
		previousTime = currentTime;
		
		cameraQuad.GetCameraVectors(camera.Orientation);
		cameraQuad.RotateCamera(window, deltaTime, currentTime);
		camera2.Position = camera.Position + camera.Orientation * (0.5f + 0.252f /*distance from screen to lens*/);// + camera.Orientation * 2.0f;
		//must be local orientation in glm::rotate function
		//glm::vec3 camera2OrientationLocal;
		//camera2OrientationLocal = glm::rotate(camera2OrientationLocal, glm::radians(rotationInputs.y), glm::normalize(glm::cross(camera2OrientationLocal, glm::vec3(0.0f, 1.0f, 0.0f))));
		glm::vec3 worldCameraDir = cameraQuad.localCameraOrientation.x * cameraQuad.mainCameraRight + cameraQuad.localCameraOrientation.y * cameraQuad.mainCameraUp + cameraQuad.localCameraOrientation.z * -cameraQuad.mainCameraForward;
		camera2.Orientation = glm::normalize(worldCameraDir);
		cameraQuad.UpdateMatrix(camera.Position, 0.5f);
		camera2.UpdateMatrix(cameraQuad.currentFov, nearClipPlane, farClipPlane);
		
		cameraQuad.ToggleDisabled(window);
		cameraQuad.AdjustExposure(window);
		cameraQuad.UpdateActiveCapture(window);

		//camera.FlyController(window);
		prevCamMatrix = camera.projection * camera.view;
		camera.UpdateMatrix(65.0f, nearClipPlane, farClipPlane);
		camera.SetMatrices(shaderProgram, "view", "projection");
		camera.SetMatrices(screenShaderProgram, "view", "projection");
		camera.Look(window);
		camera.FlyController(window);

		shadowChunker.Update(camera.Position);

		if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
			postProcessingChoice = camcorderShaderProgram;
		}
		else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
			postProcessingChoice = filmShaderProgram;
		}
		else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
			postProcessingChoice = blackAndWhiteShaderProgram;
		}

		glm::mat4 lightProj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f); //hard coded change later
		glm::mat4 lightView = glm::lookAt(-globalDirectionalLight.direction * glm::vec3(30.0) + shadowChunker.currentChunkPos, shadowChunker.currentChunkPos, glm::vec3(0.0, 1.0, 0.0));
		shadowShaderProgram.Activate();
		glUniformMatrix4fv(glGetUniformLocation(shadowShaderProgram.ID, "lightMatrix"), 1, GL_FALSE, glm::value_ptr(lightProj * lightView));
		shaderProgram.Activate();
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "lightMatrix"), 1, GL_FALSE, glm::value_ptr(lightProj * lightView));

		lightingShaderProgram.Activate();
		glUniform3f(glGetUniformLocation(lightingShaderProgram.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
		glUniform1i(glGetUniformLocation(lightingShaderProgram.ID, "skybox"), sceneSkyBox.texUnit);

		screenShaderProgram.Activate();
		glUniform3f(glGetUniformLocation(screenShaderProgram.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
		glUniform1i(glGetUniformLocation(screenShaderProgram.ID, "skybox"), sceneSkyBox.texUnit);

		sceneSkyBox.Bind();

		jitterComputeShader.Dispatch();

		ObjectHandler::scenes[ObjectHandler::currentSceneIndex].Update(deltaTime, window);
		cameraModel.sceneGameObjects[0]->transform.matrix = cameraQuad.transformMatrix;

		//-----------------------------------------------------------------------------------------> SHADOW START
		shadowMapFrameBuffer.BindFrameBuffer();
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		ObjectHandler::scenes[ObjectHandler::currentSceneIndex].DrawMeshes(shadowShaderProgram);

		shadowMapFrameBuffer.UnbindFrameBuffer();
		//-----------------------------------------------------------------------------------------> END

		glViewport(0, 0, width, height);
		
		//-----------------------------------------------------------------------------------------> GEOMETRY START
		//renders gBuffer
		basePostFrameBuffer.BindFrameBuffer();
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		shadowMapFrameBuffer.SetTexture(shadowMapFrameBuffer.depthTextures[0], lightingShaderProgram, "shadowMap");
		jitterComputeShader.SetTexture(lightingShaderProgram, "jitterMap");

		glDisable(GL_CULL_FACE);
		ObjectHandler::scenes[ObjectHandler::currentSceneIndex].DrawMeshes();
		
		//perspective 2
		camera2.SetMatrices(shaderProgram, "view", "projection");
		basePostFrameBuffer2.BindFrameBuffer();
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		ObjectHandler::scenes[ObjectHandler::currentSceneIndex].DrawMeshes();
		glEnable(GL_CULL_FACE);

		basePostFrameBuffer.UnbindFrameBuffer();
		//-----------------------------------------------------------------------------------------> END

		//screen capture (only capture on first frame R is pressed)
		cameraQuad.CaptureScreen(filterFrameBuffer.colorTextures[0], width, height, window, true);
		flashLight.SetUniforms(lightingShaderProgram);
		flashLight.position = camera2.Position;
		flashLight.direction = cameraQuad.globalCameraOrientation;

		//-----------------------------------------------------------------------------------------> LIGHTING START
		lightingFrameBuffer.BindFrameBuffer();
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		//perspective 1
		lightingFrameBuffer.SetTextureAttachment(GL_COLOR_ATTACHMENT0);
		basePostFrameBuffer.SetTexture(basePostFrameBuffer.colorTextures[0], lightingShaderProgram, "gPosition");
		basePostFrameBuffer.SetTexture(basePostFrameBuffer.colorTextures[1], lightingShaderProgram, "gLightPosition");
		basePostFrameBuffer.SetTexture(basePostFrameBuffer.colorTextures[2], lightingShaderProgram, "gNormal");
		basePostFrameBuffer.SetTexture(basePostFrameBuffer.colorTextures[3], lightingShaderProgram, "gAlbedo");
		
		basePostFrameBuffer.RenderQuad(lightingShaderProgram);
		
		skyBoxShaderProgram.Activate();
		glUniformMatrix4fv(glGetUniformLocation(skyBoxShaderProgram.ID, "camMatrix"), 1, GL_FALSE, glm::value_ptr(camera.projection * glm::mat4(glm::mat3(camera.view))));
		sceneSkyBox.Draw();

		//glDisable(GL_DEPTH_TEST);
		basePostFrameBuffer.SetTexture(cameraQuad.GetLastCapture(), screenShaderProgram, "lastCapture");
		glUniform1f(glGetUniformLocation(screenShaderProgram.ID, "lastCaptureTime"), cameraQuad.lastCaptureTime);

		//std::cout << currentTime << " " << cameraQuad.lastCaptureTime << std::endl;
		//if (currentTime - 0.5f < cameraQuad.lastCaptureTime) {
			//std::cout << "true" << std::endl;
		//}

		//render camera screen
		glUniform1f(glGetUniformLocation(screenShaderProgram.ID, "currentTime"), currentTime);
		//cameraQuad.RenderMainQuad(filterFrameBuffer.colorTextures[0], screenShaderProgram);
		shadowMapFrameBuffer.SetTexture(shadowMapFrameBuffer.depthTextures[0], screenShaderProgram, "shadowMap");
		jitterComputeShader.SetTexture(screenShaderProgram, "jitterMap");
		basePostFrameBuffer.SetTexture(cameraQuad.fullscreen ? cameraQuad.GetActiveCapture() : filterFrameBuffer.colorTextures[0], screenShaderProgram, "cameraImage");
		basePostFrameBuffer.SetTexture(cameraScreenUVMap, screenShaderProgram, "screenUVMap");
		cameraModel.sceneGameObjects[0]->meshes[7].material->shader = screenShaderProgram;
		cameraModel.sceneGameObjects[0]->meshes[7].material->SetTextures();
		//glUniform1f(glGetUniformLocation(screenShaderProgram.ID, "albedoScale"), 1.0f);
		cameraModel.sceneGameObjects[0]->meshes[7].visible = true;

		cameraModel.sceneGameObjects[0]->meshes[7].Draw(cameraQuad.transformMatrix, screenShaderProgram);

		cameraModel.sceneGameObjects[0]->meshes[7].visible = false;
		cameraModel.sceneGameObjects[0]->meshes[7].material->shader = shaderProgram;
		//end render camera screen

		glDisable(GL_DEPTH_TEST);
		captureScreenShaderProgram.Activate();
		glUniform1f(glGetUniformLocation(captureScreenShaderProgram.ID, "time"), currentTime);
		cameraQuad.RenderSecondQuad(cameraQuad.GetActiveCapture(), captureScreenShaderProgram);
		glEnable(GL_DEPTH_TEST);
		
		//perspective 2
		lightingFrameBuffer2.BindFrameBuffer();
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		basePostFrameBuffer.SetTexture(basePostFrameBuffer2.colorTextures[0], lightingShaderProgram, "gPosition");
		basePostFrameBuffer.SetTexture(basePostFrameBuffer2.colorTextures[1], lightingShaderProgram, "gLightPosition");
		basePostFrameBuffer.SetTexture(basePostFrameBuffer2.colorTextures[2], lightingShaderProgram, "gNormal");
		basePostFrameBuffer.SetTexture(basePostFrameBuffer2.colorTextures[3], lightingShaderProgram, "gAlbedo");

		basePostFrameBuffer.RenderQuad(lightingShaderProgram);
		//SWITCH CAMERA
		skyBoxShaderProgram.Activate();
		glUniformMatrix4fv(glGetUniformLocation(skyBoxShaderProgram.ID, "camMatrix"), 1, GL_FALSE, glm::value_ptr(camera2.projection * glm::mat4(glm::mat3(camera2.view))));
		sceneSkyBox.Draw();

		lightingFrameBuffer.UnbindFrameBuffer();
		//-----------------------------------------------------------------------------------------> END
		//perspective 1
		bloomRenderer.RenderBloomTexture(bloomUpsampleShaderProgram, bloomDownsampleShaderProgram, lightingFrameBuffer.colorTextures[0].textureID, lightingFrameBuffer.colorTextures[0].textureUnit, 0.005f);
		//perspective 2 (must use a second bloom renderer because this view is lower resolution)
		bloomRenderer2.RenderBloomTexture(bloomUpsampleShaderProgram, bloomDownsampleShaderProgram, lightingFrameBuffer2.colorTextures[0].textureID, lightingFrameBuffer2.colorTextures[0].textureUnit, 0.005f);
		
		//-----------------------------------------------------------------------------------------> FOG START
		//render fog
		fogFrameBuffer.BindFrameBuffer();
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		fogShaderProgram.Activate();
		//perspective 1
		basePostFrameBuffer.SetTexture(lightingFrameBuffer.colorTextures[0], fogShaderProgram, "renderedScene");
		basePostFrameBuffer.SetTexture(lightingFrameBuffer.depthTextures[0], fogShaderProgram, "renderedSceneDepth");

		basePostFrameBuffer.RenderQuad(fogShaderProgram);
		//perspective 2
		fogFrameBuffer2.BindFrameBuffer();
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		basePostFrameBuffer.SetTexture(lightingFrameBuffer2.colorTextures[0], fogShaderProgram, "renderedScene");
		basePostFrameBuffer.SetTexture(lightingFrameBuffer2.depthTextures[0], fogShaderProgram, "renderedSceneDepth");

		basePostFrameBuffer.RenderQuad(fogShaderProgram);
		/*
		glDisable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		*/

		fogFrameBuffer.UnbindFrameBuffer();
		//-----------------------------------------------------------------------------------------> END

		//-----------------------------------------------------------------------------------------> TONEMAP START
		//render scene with regular tonemapping
		toneMapFrameBuffer.BindFrameBuffer();
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		tonemapperShaderProgram.Activate();

		//glUniform1i(glGetUniformLocation(tonemapperShaderProgram.ID, "renderedScene"), fogFrameBuffer.colorTextures[0].textureUnit);
		//glActiveTexture(GL_TEXTURE0 + fogFrameBuffer.colorTextures[0].textureUnit);
		//glBindTexture(GL_TEXTURE_2D, fogFrameBuffer.colorTextures[0].textureID);

		//perspective 1
		basePostFrameBuffer.SetTexture(fogFrameBuffer.colorTextures[0], tonemapperShaderProgram, "renderedScene");
		if (camera.useAutoExposure) {
			float measuredLuminance = pow(camera.GetSceneLuminance(width, height) * 2, 2.0f) / 2.0f;
			float desiredExposure = 0.03f / (measuredLuminance + 0.0001f);
			camera.exposure += (desiredExposure - camera.exposure) * deltaTime * 5.0f; //interpolate toward 0.08
		}
		//exposure = measuredExposure + (0.08f - measuredExposure) * 0.1f;
		//std::cout << exposure << std::endl;
		glUniform1f(glGetUniformLocation(tonemapperShaderProgram.ID, "bloomStrength"), camera.exposure);

		basePostFrameBuffer.SetTexture(fogFrameBuffer.depthTextures[0], tonemapperShaderProgram, "renderedSceneDepth");

		glUniform1i(glGetUniformLocation(tonemapperShaderProgram.ID, "bloomBlur"), bloomRenderer.mipTexUnit);
		glActiveTexture(GL_TEXTURE0 + bloomRenderer.mipTexUnit);
		glBindTexture(GL_TEXTURE_2D, bloomRenderer.BloomTexture());

		basePostFrameBuffer.RenderQuad(tonemapperShaderProgram);

		//perspective 2
		toneMapFrameBuffer2.BindFrameBuffer();
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		basePostFrameBuffer.SetTexture(fogFrameBuffer2.colorTextures[0], tonemapperShaderProgram, "renderedScene");
		if (camera2.useAutoExposure) {
			float measuredLuminance = pow(camera2.GetSceneLuminance(width / 8, height / 8) * 2, 2.0f) / 2.0f;
			float desiredExposure = (cameraQuad.defaultExposure - 0.01f) / (measuredLuminance + 0.0001f);
			camera2.exposure += (desiredExposure - camera2.exposure) * deltaTime * 5.0f; //interpolate toward 0.08
		}
		//exposure = measuredExposure + (0.08f - measuredExposure) * 0.1f;
		//std::cout << exposure << std::endl;
		glUniform1f(glGetUniformLocation(tonemapperShaderProgram.ID, "bloomStrength"), camera2.exposure);

		basePostFrameBuffer.SetTexture(fogFrameBuffer2.depthTextures[0], tonemapperShaderProgram, "renderedSceneDepth");

		glUniform1i(glGetUniformLocation(tonemapperShaderProgram.ID, "bloomBlur"), bloomRenderer2.mipTexUnit);
		glActiveTexture(GL_TEXTURE0 + bloomRenderer2.mipTexUnit);
		glBindTexture(GL_TEXTURE_2D, bloomRenderer2.BloomTexture());

		basePostFrameBuffer.RenderQuad(tonemapperShaderProgram);

		toneMapFrameBuffer.UnbindFrameBuffer();
		//-----------------------------------------------------------------------------------------> END

		//-----------------------------------------------------------------------------------------> FILTER START
		 
		//render scene with filter (to be used in next frame)
		filterFrameBuffer.BindFrameBuffer();
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		postProcessingChoice.Activate();
		glUniform1f(glGetUniformLocation(postProcessingChoice.ID, "time"), currentTime);
		basePostFrameBuffer.SetTexture(basePostFrameBuffer2.colorTextures[0], postProcessingChoice, "gPosition");
		glUniformMatrix4fv(glGetUniformLocation(postProcessingChoice.ID, "prevViewProjMatrix"), 1, GL_FALSE, glm::value_ptr(prevCamMatrix));
		glUniformMatrix4fv(glGetUniformLocation(postProcessingChoice.ID, "camMatrix"), 1, GL_FALSE, glm::value_ptr(camera2.projection * camera2.view));

		glUniform1i(glGetUniformLocation(postProcessingChoice.ID, "isCapturing"), cameraQuad.captureDelayFrame);

		basePostFrameBuffer.SetTexture(toneMapFrameBuffer2.colorTextures[0], postProcessingChoice, "renderedScene");
		basePostFrameBuffer.SetTexture(toneMapFrameBuffer2.depthTextures[0], postProcessingChoice, "renderedSceneDepth");
		basePostFrameBuffer.RenderQuad(postProcessingChoice);

		filterFrameBuffer.UnbindFrameBuffer();
		//-----------------------------------------------------------------------------------------> END

		//render regular scene to actually be shown on screen
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		basePostShaderProgram.Activate();
		//basePostFrameBuffer.SetTexture(cameraQuad.fullscreen ? cameraQuad.GetLastCapture() : toneMapFrameBuffer.colorTextures[0], basePostShaderProgram, "renderedScene");
		basePostFrameBuffer.SetTexture(toneMapFrameBuffer.colorTextures[0], basePostShaderProgram, "renderedScene");
		//basePostFrameBuffer.SetTexture(cameraQuad.GetLastCapture(), basePostShaderProgram, "renderedScene");
		basePostFrameBuffer.SetTexture(toneMapFrameBuffer.depthTextures[0], basePostShaderProgram, "renderedSceneDepth");
		basePostFrameBuffer.RenderQuad(basePostShaderProgram);
		if (cameraQuad.maxFullscreen) {
			basePostFrameBuffer.SetTexture(cameraQuad.GetActiveCapture(), captureScreenShaderProgram, "renderedScene");
			basePostFrameBuffer.RenderQuad(captureScreenShaderProgram);
		}


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	delete Texture::defaultAlbedo;
	delete Texture::defaultNormalMap;
	delete defaultTransparent;

	glfwTerminate();
	return 0;
}