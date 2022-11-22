#ifndef GLAD_H
#define GLAD_H
#include <glad/glad.h>
#endif
#include <GLFW/glfw3.h>

#include "Window.h"
#include "Shader.h"
#include "ScreenSpaceShader.h"
#include "Texture.h"

#include "VolumetricCloud.h"
#include "Terrain.h"
#include "Skybox.h"
#include "Water.h"
#include "CloudModel.h"

#include <camera.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>

#include "GlError.h"

#include "SceneElements.h"
#include "DrawableObject.h"

#include <iostream>
#include <vector>
#include <functional>

int main()
{
	glm::vec3 startPosition(0.0f, 800.0f, 0.0f);
	Camera camera(startPosition);

	bool success;
	Window window(success);
	if (!success) return -1;

	window.camera = &camera;

	glm::vec3 fogColor(0.5, 0.6, 0.7);
	glm::vec3 lightColor(255, 255, 230);
	lightColor /= 255.0;

	FrameBufferObject SceneFBO(Window::SCR_WIDTH, Window::SCR_HEIGHT);
	glm::vec3 lightPosition, seed;
	glm::mat4 proj = glm::perspective(glm::radians(camera.Zoom), (float)Window::SCR_WIDTH / (float)Window::SCR_HEIGHT, 5.f, 10000000.0f);
	glm::vec3 lightDir = glm::vec3(-.5, 0.5, 1.0);

	SceneElements scene;
	scene.lightPos = lightPosition;
	scene.lightColor = lightColor;
	scene.fogColor = fogColor;
	scene.seed = seed;
	scene.projMatrix = proj;
	scene.cam = &camera;
	scene.sceneFBO = &SceneFBO;
	scene.lightDir = lightDir;

	DrawableObject::scene = &scene;

	int gridLength = 120;
	Terrain terrain(gridLength);

	float waterHeight = 120.;
	Water water(glm::vec2(0.0, 0.0), gridLength, waterHeight);
	terrain.waterPtr = &water;

	Skybox skybox;
	CloudModel cloudsModel(&scene, &skybox);

	VolumetricCloud volumetricCloud(Window::SCR_WIDTH, Window::SCR_HEIGHT, &cloudsModel);
	VolumetricCloud reflectionVolumetricCloud(1280, 720, &cloudsModel); 

	ScreenSpaceShader PostProcessing("shaders/post_processing.frag");
	ScreenSpaceShader fboVisualizer("shaders/visualizeFbo.frag");

	while (window.continueLoop())
	{
		scene.lightDir = glm::normalize(scene.lightDir);
		scene.lightPos = scene.lightDir * 1e6f + camera.Position;

		window.processInput(1. / 30);

		terrain.updateTilesPositions();
		cloudsModel.update();
		skybox.update();

		SceneFBO.bind();

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		glClearColor(fogColor[0], fogColor[1], fogColor[2], 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		if (scene.wireframe) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		glm::mat4 view = scene.cam->GetViewMatrix();
		scene.projMatrix = glm::perspective(glm::radians(camera.Zoom), (float)Window::SCR_WIDTH / (float)Window::SCR_HEIGHT, 5.f, 10000000.0f);

		water.bindReflectionFBO();
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		scene.cam->invertPitch();
		scene.cam->Position.y -= 2 * (scene.cam->Position.y - water.getHeight());

		terrain.up = 1.0;
		terrain.draw();
		FrameBufferObject const& reflFBO = water.getReflectionFBO();

		ScreenSpaceShader::disableTests();

		reflectionVolumetricCloud.draw();
		water.bindReflectionFBO(); 


		Shader& post = PostProcessing.getShader();
		post.use();
		post.setVec2("resolution", glm::vec2(1280, 720));
		post.setSampler2D("screenTexture", reflFBO.tex, 0);
		post.setSampler2D("depthTex", reflFBO.depthTex, 2);
		post.setSampler2D("cloudTEX", reflectionVolumetricCloud.getCloudsRawTexture(), 1);
		PostProcessing.draw();

		ScreenSpaceShader::enableTests();

		scene.cam->invertPitch();
		scene.cam->Position.y += 2 * abs(scene.cam->Position.y - water.getHeight());

		water.bindRefractionFBO();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		terrain.up = -1.0;
		terrain.draw();

		scene.sceneFBO->bind();
		terrain.draw();
		water.draw();

		ScreenSpaceShader::disableTests();

		volumetricCloud.draw();
		skybox.draw();

		unbindCurrentFrameBuffer(); 
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		post.use();
		post.setVec2("resolution", glm::vec2(Window::SCR_WIDTH, Window::SCR_HEIGHT));
		post.setVec3("cameraPosition", scene.cam->Position);
		post.setSampler2D("screenTexture", SceneFBO.tex, 0);
		post.setSampler2D("cloudTEX", volumetricCloud.getCloudsTexture(), 1);
		post.setSampler2D("depthTex", SceneFBO.depthTex, 2);
		post.setSampler2D("cloudDistance", volumetricCloud.getCloudsTexture(VolumetricCloud::cloudDistance), 3);

		post.setBool("wireframe", scene.wireframe);

		post.setMat4("VP", scene.projMatrix * view);
		PostProcessing.draw();

		Shader& fboVisualizerShader = fboVisualizer.getShader();
		fboVisualizerShader.use();
		fboVisualizerShader.setSampler2D("fboTex", volumetricCloud.getCloudsTexture(), 0);

		window.swapBuffersAndPollEvents();
	}
}