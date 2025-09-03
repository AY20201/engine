#include"../engine_headers/CameraQuad.h"

CameraQuad::CameraQuad(float width, float height, glm::vec3 initialOrientation, Light* flashLight) {
	emptyTexObject = TextureObject{ 0, 0 };
	CameraQuad::flashLight = flashLight;

	float xCorner = width / 2.0f;
	float yCorner = height / 2.0f;
	localCameraOrientation = initialOrientation;
	orientationNoVerticalRotation = localCameraOrientation;

	Vertex quadVertices[] =
	{
		Vertex{glm::vec3(-xCorner, -yCorner,  0.0f), glm::vec2(0.0f, 0.0f)}, //bottom left
		Vertex{glm::vec3(xCorner, -yCorner,  0.0f), glm::vec2(1.0f, 0.0f)}, //bottom right
		Vertex{glm::vec3(xCorner, yCorner,  0.0f), glm::vec2(1.0f, 1.0f)}, //top right
		Vertex{glm::vec3(-xCorner, yCorner,  0.0f), glm::vec2(0.0f, 1.0f)}, //top left
	};

	GLuint quadIndices[] =
	{
		0, 1, 2,
		2, 3, 0
	};

	std::vector<Vertex> quadVerts = std::vector<Vertex>(quadVertices, quadVertices + sizeof(quadVertices) / sizeof(Vertex));
	std::vector<GLuint> quadInds = std::vector<GLuint>(quadIndices, quadIndices + sizeof(quadIndices) / sizeof(GLuint));
	quadMesh = Mesh(quadVerts, quadInds, nullptr, true, false, false);

	glm::vec2 offset = glm::vec2(0.005f, 0.005f) * glm::vec2(height, width);
	glm::vec2 bottomLeft = glm::vec2(0.85f - offset.x, 0.0f + offset.y) * 2.0f - glm::vec2(1.0f, 1.0f);//glm::vec2(0.75f, -1.0f);
	glm::vec2 topRight = glm::vec2(1.0f - offset.x, 0.15f + offset.y) * 2.0f - glm::vec2(1.0f, 1.0f);//glm::vec2(1.0f, -0.75f);
	const Vertex secondQuadVertices[] =
	{
		Vertex{glm::vec3(bottomLeft.x, bottomLeft.y,  0.0f), glm::vec2(0.0f, 0.0f)},
		Vertex{glm::vec3(topRight.x, topRight.y,  0.0f), glm::vec2(1.0f, 1.0f)},
		Vertex{glm::vec3(bottomLeft.x, topRight.y,  0.0f), glm::vec2(0.0f, 1.0f)},

		Vertex{glm::vec3(bottomLeft.x, bottomLeft.y,  0.0f), glm::vec2(0.0f, 0.0f)},
		Vertex{glm::vec3(topRight.x, bottomLeft.y,  0.0f), glm::vec2(1.0f, 0.0f)},
		Vertex{glm::vec3(topRight.x, topRight.y,  0.0f), glm::vec2(1.0f, 1.0f)}
	};

	std::vector<Vertex> secondQuadVerts = std::vector<Vertex>(secondQuadVertices, secondQuadVertices + sizeof(secondQuadVertices) / sizeof(Vertex));

	secondQuadVAO.Bind();
	VBO vbo(secondQuadVerts);

	secondQuadVAO.LinkAttrib(vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
	secondQuadVAO.LinkAttrib(vbo, 1, 2, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
	secondQuadVAO.LinkAttrib(vbo, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)(5 * sizeof(float)));
	secondQuadVAO.LinkAttrib(vbo, 3, 3, GL_FLOAT, sizeof(Vertex), (void*)(8 * sizeof(float)));

	secondQuadVAO.Unbind();
	vbo.Unbind();
}

void CameraQuad::AdjustFOV(float scrollValue) {
	targetFov -= scrollValue * 6.0f;
	targetFov = std::max(std::min(targetFov, 130.0f), 10.0f);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	CameraQuad* camQuad = static_cast<CameraQuad*>(glfwGetWindowUserPointer(window));
	if (camQuad)
		camQuad->AdjustFOV((float)yoffset);
}

void CameraQuad::RotateCamera(GLFWwindow* window, float deltaTime, float currentTime) {
	glm::vec2 inputs = glm::vec2(0.0f); //right/left, up/down

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		inputs.x += 1.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		inputs.x -= 1.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		inputs.y += 1.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		inputs.y -= 1.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		targetNeutralRotation = true;
		targetFov = 55.0f;
		defaultExposure = 0.02f;
	}
	glfwSetScrollCallback(window, scroll_callback);
	//lerp back to neutral orientation after pressing space bar
	currentFov += (targetFov - currentFov) * 6.0f * deltaTime;

	if (targetNeutralRotation) {
		localCameraOrientation = localCameraOrientation + (glm::vec3(0.0f, 0.0f, -1.0f) - localCameraOrientation) * 6.0f * deltaTime;
		if (abs(glm::angle(localCameraOrientation, glm::vec3(0.0f, 0.0f, -1.0f))) <= glm::radians(1.0f)) {
			localCameraOrientation = glm::vec3(0.0f, 0.0f, -1.0f);
			orientationNoVerticalRotation = glm::vec3(0.0f, 0.0f, -1.0f);
			targetNeutralRotation = false;
		}
		return;
	}

	//same thing as camera look function
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 orientation = glm::rotate(localCameraOrientation, glm::radians(inputs.y), glm::normalize(glm::cross(localCameraOrientation, up)));
	//check to prevent over rotating
	if (abs(glm::angle(orientation, up) - glm::radians(90.0f)) <= glm::radians(45.0f))
	{
		localCameraOrientation = orientation;
	}

	glm::vec3 horizontalOrientation = glm::rotate(orientationNoVerticalRotation, glm::radians(-inputs.x), up);
	float angleDifference = glm::angle(horizontalOrientation, glm::vec3(1.0f, 0.0f, 0.0f)) - glm::radians(90.0f);
	//if (abs(angleDifference) <= glm::radians(45.0f))
	//{
	//if outside of range but moving back in OR in range
	if ((angleDifference > glm::radians(45.0f) && !(-inputs.x == 1.0f)) || (angleDifference < glm::radians(-45.0f) && !(-inputs.x == -1.0f)) || abs(angleDifference) <= glm::radians(45.0f)) {
		localCameraOrientation = glm::rotate(localCameraOrientation, glm::radians(-inputs.x), up);
		orientationNoVerticalRotation = horizontalOrientation;
	}
	//}
	CheckFlash(deltaTime);
	CameraQuad::currentTime = currentTime;
}

void CameraQuad::ToggleDisabled(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && !disabledKeyPressed) {
		disabled = !disabled;
		disabledKeyPressed = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE) {
		disabledKeyPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && !fullscreenKeyPressed) {
		fullscreen = !fullscreen;
		if (maxFullscreen) { 
			maxFullscreen = false; 
			fullscreen = false;
		}
		fullscreenKeyPressed = true;
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
			maxFullscreen = true;
		}
	}
	else if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE) {
		fullscreenKeyPressed = false;
	}
}

void CameraQuad::GetCameraVectors(glm::vec3 cameraOrientation) {
	mainCameraForward = glm::normalize(cameraOrientation);
	mainCameraRight = glm::normalize(glm::cross(mainCameraForward, glm::vec3(0.0f, 1.0f, 0.0f)));
	mainCameraUp = glm::cross(mainCameraRight, mainCameraForward);
}

void CameraQuad::UpdateMatrix(glm::vec3 mainCameraPos, float offset) {
	globalCameraOrientation = localCameraOrientation.x * mainCameraRight + localCameraOrientation.y * mainCameraUp + localCameraOrientation.z * -mainCameraForward;
	glm::vec3 globalForward = glm::normalize(globalCameraOrientation);
	glm::vec3 globalRight = glm::normalize(glm::cross(globalForward, mainCameraUp));
	glm::vec3 globalUp = glm::cross(globalRight, globalForward);

	quadPos = mainCameraPos + mainCameraForward * offset;
	transformMatrix = glm::mat4(0.0f);
	transformMatrix[0] = glm::vec4(globalRight * quadScale, 0.0f);
	transformMatrix[1] = glm::vec4(globalUp * quadScale, 0.0f);
	transformMatrix[2] = glm::vec4(-globalForward * quadScale, 0.0f);
	transformMatrix[3] = glm::vec4(quadPos, 1.0f);
}

void CameraQuad::RenderMainQuad(TextureObject& cameraRender, Shader& shader) {
	if (disabled) { return; }

	shader.Activate();
	glUniform1i(glGetUniformLocation(shader.ID, "albedo"), cameraRender.textureUnit);
	glUniform1f(glGetUniformLocation(shader.ID, "albedoScale"), 1.0f);
	glActiveTexture(GL_TEXTURE0 + cameraRender.textureUnit);
	glBindTexture(GL_TEXTURE_2D, cameraRender.textureID);

	quadMesh.Draw(transformMatrix, shader);
}

void CameraQuad::RenderSecondQuad(TextureObject& captureImage, Shader& shader) {
	shader.Activate();
	if (captureImage.textureID == 0) {
		glUniform1i(glGetUniformLocation(shader.ID, "empty"), 0);
	}
	else {
		glUniform1i(glGetUniformLocation(shader.ID, "empty"), 1);
		glUniform1i(glGetUniformLocation(shader.ID, "renderedScene"), captureImage.textureUnit);
		glActiveTexture(GL_TEXTURE0 + captureImage.textureUnit);
		glBindTexture(GL_TEXTURE_2D, captureImage.textureID);
	}
	
	secondQuadVAO.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void CameraQuad::CaptureScreen(TextureObject& captureTexture, int screenWidth, int screenHeight, GLFWwindow* window, bool useFlash) {
	if (!captureKeyPressed && glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		//wait for the delay, then delay one more frame
		if (!captureDelayFrame) { 
			captureDelayFrame = true;
			flashLight->intensity = 1.0f;
			flashStarted = currentTime;
			return;
		}
		else {
			captureDelayFrame = false;
		}
		/*actual capture code*/
		GLuint captureID;
		glGenTextures(1, &captureID);

		GLuint captureTexUnit = Texture::activeTexUnit;
		Texture::activeTexUnit++;

		glBindTexture(GL_TEXTURE_2D, captureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		//glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 0, 0, screenWidth, screenHeight, 0);
		glCopyImageSubData(
			captureTexture.textureID, GL_TEXTURE_2D, 0, // src name, target, level
			0, 0, 0,                         // src x,y,z
			captureID, GL_TEXTURE_2D, 0,     // dst name, target, level
			0, 0, 0,                         // dst x,y,z
			screenWidth, screenHeight, 1     // region size
		);

		activeCapture++;
		//ClearCapture(0);
		screenCaptures.push_back(TextureObject{ captureID, captureTexUnit });
		//std::cout << captureID << " " << captureTexUnit << " " << std::endl;

		glBindTexture(GL_TEXTURE_2D, 0);
		/*actual capture code*/
		captureKeyPressed = true;
		lastCaptureTime = currentTime;
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE) {
		captureKeyPressed = false;
	}
}

TextureObject& CameraQuad::GetLastCapture() {
	if (screenCaptures.size() > 0) {
		return screenCaptures.back();
	}
	return emptyTexObject;
}

void CameraQuad::UpdateActiveCapture(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS && !arrowKeyPressed) {
		activeCapture--;
		if (activeCapture < 0) { activeCapture = screenCaptures.size() - 1; }
		arrowKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS && !arrowKeyPressed) {
		activeCapture++;
		if (activeCapture >= screenCaptures.size()) { activeCapture = 0; }
		arrowKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_K) == GLFW_RELEASE) {
		arrowKeyPressed = false;
	}
}

TextureObject& CameraQuad::GetActiveCapture() {
	if (screenCaptures.size() == 0) { return emptyTexObject; }
	return screenCaptures[activeCapture];
}

void CameraQuad::AdjustExposure(GLFWwindow* window) {
	float exposureAdjustmentSize = 0.002f;

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && !exposureKeyPressed) {
		defaultExposure += exposureAdjustmentSize;
		if (defaultExposure < 0.005f) { defaultExposure = 0.005f; }
		exposureKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && !exposureKeyPressed) {
		defaultExposure -= exposureAdjustmentSize;
		if (defaultExposure >= 0.055f) { defaultExposure = 0.055f; }
		exposureKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE) {
		exposureKeyPressed = false;
	}
}

void CameraQuad::ClearCapture(int index) {
	if (index < screenCaptures.size()) {
		TextureObject deletedCapture = screenCaptures[index];
		screenCaptures.erase(screenCaptures.begin() + index);
		glDeleteTextures(1, &deletedCapture.textureID);
	}
}

void CameraQuad::CheckFlash(float deltaTime) {
	currentTime += deltaTime;
	if (currentTime - flashStarted > flashTime) {
		flashLight->intensity = 0.0f;
	}
}