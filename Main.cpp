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
#include"engine_headers/Skybox.h"
#include"engine_headers/MeshScene.h"
#include"engine_headers/BloomRenderer.h"


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

	Texture::defaultAlbedo = defaultAlbedo;
	Texture::defaultNormalMap = defaultNormalMap;

	FrameBufferObject lightingFrameBuffer(width, height, 1, 1);
	FrameBufferObject fogFrameBuffer(width, height, 1, 1);
	FrameBufferObject basePostFrameBuffer(width, height);
	FrameBufferObject finalPassFrameBuffer(width, height, 1, 1);
	FrameBufferObject shadowMapFrameBuffer(2048, 2048, 1, 1);
	basePostFrameBuffer.SetUpGBuffer();
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

	BloomRenderer bloomRenderer(width, height, 7);
	
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

	Camera camera(width, height, glm::vec3(0.0f, 1.0f, 3.0f));

	ObjectHandler::currentSceneIndex = 0;
	ObjectHandler::scenes.push_back(ObjectHandler());

	MeshScene monkey(Transform::Zero, std::vector<Behavior*>{ nullptr }, std::vector<const char*>{ "../../engine_resource/3D Objects/monkey/monkey.obj" }, shaderProgram, nullptr, false, false, false);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	double currentTime = glfwGetTime();
	double previousTime = glfwGetTime();
	float deltaTime = 0.0f;

	LightHandler::Instance.SetLightUniforms(lightingShaderProgram);

	ObjectHandler::scenes[ObjectHandler::currentSceneIndex].Awake();

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		currentTime = glfwGetTime();
		deltaTime = (float)currentTime - (float)previousTime;
		previousTime = currentTime;

		//camera.FlyController(window);
		camera.UpdateMatrix(55.0f, nearClipPlane, farClipPlane);
		camera.SetMatrices(shaderProgram, "view", "projection");
		camera.Look(window);
		camera.FlyController(window);

		glm::mat4 lightProj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f); //hard coded change later
		glm::mat4 lightView = glm::lookAt(-globalDirectionalLight.direction * glm::vec3(30.0), glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		shadowShaderProgram.Activate();
		glUniformMatrix4fv(glGetUniformLocation(shadowShaderProgram.ID, "lightMatrix"), 1, GL_FALSE, glm::value_ptr(lightProj * lightView));
		shaderProgram.Activate();
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "lightMatrix"), 1, GL_FALSE, glm::value_ptr(lightProj * lightView));

		lightingShaderProgram.Activate();
		glUniform3f(glGetUniformLocation(lightingShaderProgram.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
		glUniform1i(glGetUniformLocation(lightingShaderProgram.ID, "skybox"), sceneSkyBox.texUnit);

		sceneSkyBox.Bind();

		jitterComputeShader.Dispatch();

		ObjectHandler::scenes[ObjectHandler::currentSceneIndex].Update(deltaTime, window);

		shadowMapFrameBuffer.BindFrameBuffer();
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		ObjectHandler::scenes[ObjectHandler::currentSceneIndex].DrawMeshes(shadowShaderProgram);

		shadowMapFrameBuffer.UnbindFrameBuffer();
		//glCullFace(GL_BACK);
		//
		glViewport(0, 0, width, height);
		//renders gBuffer
		basePostFrameBuffer.BindFrameBuffer();
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		shadowMapFrameBuffer.SetTexture(shadowMapFrameBuffer.depthTextures[0], lightingShaderProgram, "shadowMap");
		jitterComputeShader.SetTexture(lightingShaderProgram, "jitterMap");

		glDisable(GL_CULL_FACE);
		ObjectHandler::scenes[ObjectHandler::currentSceneIndex].DrawMeshes();
		glEnable(GL_CULL_FACE);

		basePostFrameBuffer.UnbindFrameBuffer();

		skyBoxShaderProgram.Activate();
		glUniformMatrix4fv(glGetUniformLocation(skyBoxShaderProgram.ID, "camMatrix"), 1, GL_FALSE, glm::value_ptr(camera.projection * glm::mat4(glm::mat3(camera.view))));

		lightingFrameBuffer.BindFrameBuffer();
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		basePostFrameBuffer.SetTexture(basePostFrameBuffer.colorTextures[0], lightingShaderProgram, "gPosition");
		basePostFrameBuffer.SetTexture(basePostFrameBuffer.colorTextures[1], lightingShaderProgram, "gLightPosition");
		basePostFrameBuffer.SetTexture(basePostFrameBuffer.colorTextures[2], lightingShaderProgram, "gNormal");
		basePostFrameBuffer.SetTexture(basePostFrameBuffer.colorTextures[3], lightingShaderProgram, "gAlbedo");
		
		basePostFrameBuffer.RenderQuad(lightingShaderProgram);

		sceneSkyBox.Draw();
		
		lightingFrameBuffer.UnbindFrameBuffer();

		bloomRenderer.RenderBloomTexture(bloomUpsampleShaderProgram, bloomDownsampleShaderProgram, lightingFrameBuffer.colorTextures[0].textureID, lightingFrameBuffer.colorTextures[0].textureUnit, 0.005f);

		fogFrameBuffer.BindFrameBuffer();
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		fogShaderProgram.Activate();
		basePostFrameBuffer.SetTexture(lightingFrameBuffer.colorTextures[0], fogShaderProgram, "renderedScene");
		basePostFrameBuffer.SetTexture(lightingFrameBuffer.depthTextures[0], fogShaderProgram, "renderedSceneDepth");

		basePostFrameBuffer.RenderQuad(fogShaderProgram);
		
		glDisable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);

		fogFrameBuffer.UnbindFrameBuffer();

		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		tonemapperShaderProgram.Activate();

		glUniform1i(glGetUniformLocation(tonemapperShaderProgram.ID, "renderedScene"), fogFrameBuffer.colorTextures[0].textureUnit);
		glActiveTexture(GL_TEXTURE0 + fogFrameBuffer.colorTextures[0].textureUnit);
		glBindTexture(GL_TEXTURE_2D, fogFrameBuffer.colorTextures[0].textureID);

		glUniform1i(glGetUniformLocation(tonemapperShaderProgram.ID, "bloomBlur"), bloomRenderer.mipTexUnit);
		glActiveTexture(GL_TEXTURE0 + bloomRenderer.mipTexUnit);
		glBindTexture(GL_TEXTURE_2D, bloomRenderer.BloomTexture());

		basePostFrameBuffer.RenderQuad(tonemapperShaderProgram);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	delete Texture::defaultAlbedo;
	delete Texture::defaultNormalMap;
	delete defaultTransparent;

	glfwTerminate();
	return 0;
}