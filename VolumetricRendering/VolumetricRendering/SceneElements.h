#pragma once

#include <camera.h>
#include <glm/glm.hpp>
#include "Buffers.h"
#include <random>

struct SceneElements {

	glm::vec3 lightPos, lightColor, lightDir, fogColor, seed;
	glm::mat4 projMatrix;
	Camera* cam;
	FrameBufferObject* sceneFBO;
	bool wireframe = false;
};