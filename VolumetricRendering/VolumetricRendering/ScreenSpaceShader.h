#pragma once
#include "Shader.h"
#include <glad/glad.h>
#include "DrawableObject.h"

class ScreenSpaceShader : DrawableObject
{
public:
	ScreenSpaceShader(const char* fragmentPath);
	~ScreenSpaceShader() = default;

	Shader* const getShaderPtr() {
		return shad;
	}

	Shader& const getShader() {
		return *shad;
	}

	virtual void draw();
	static void drawQuad();

	static void disableTests() {
		glDisable(GL_DEPTH_TEST);
	}

	static void enableTests() {
		glEnable(GL_DEPTH_TEST);
	}

private:
	Shader* shad;
	static unsigned int quadVAO, quadVBO;
	static bool initialized;

	void initializeQuad();
};