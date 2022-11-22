#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>
#include "Shader.h"
#include "Buffers.h"
#include <GLFW/glfw3.h>
#include "Window.h"
#include "drawableObject.h"

class Water : public DrawableObject
{
public:
	Water(glm::vec2 position, float scale, float height);
	virtual ~Water() = default;
	virtual void draw();
	void bindRefractionFBO();
	void bindReflectionFBO();
	void unbindFBO();

	void setPosition(glm::vec2 position, float scale, float height);

	void setHeight(float height);

	float getHeight();

	glm::mat4 getModelMatrix();

	static const int FBOw = 1280;
	static const int FBOh = 720;

	FrameBufferObject const& getReflectionFBO();

private:
	void drawVertices();

	unsigned int m_planeVAO;
	unsigned int m_planeVBO;
	unsigned int m_planeEBO;
	float m_scale; 
	float m_height;
	FrameBufferObject* m_reflectionFBO;
	FrameBufferObject* m_refractionFBO;

	unsigned int m_dudvMap;
	unsigned int m_normalMap;
	glm::mat4 m_modelMatrix;
	Shader* m_shader;
};