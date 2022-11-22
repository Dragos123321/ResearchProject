#include "Water.h"
#include "sceneElements.h"
#include "Terrain.h"
#include "Tools.h"

Water::Water(glm::vec2 position, float scale, float height) : m_scale{ scale }, m_height{ height }
{
	m_shader = new Shader("WaterShader");
	m_shader->attachShader("shaders/water.vert")
		->attachShader("shaders/water.frag")
		->linkPrograms();


	glm::mat4 identity;
	glm::mat4 scaleMatrix = glm::scale(identity, glm::vec3(scale, scale, scale));
	glm::mat4 transMatrix = glm::translate(identity, glm::vec3(position.x, height, position.y));
	m_modelMatrix = transMatrix * scaleMatrix;

	m_reflectionFBO = new FrameBufferObject(FBOw, FBOh);
	m_refractionFBO = new FrameBufferObject(FBOw, FBOh);

	initializePlaneVAO(2, Terrain::tileW, &m_planeVAO, &m_planeVBO, &m_planeEBO);
}

void Water::bindReflectionFBO() 
{
	m_reflectionFBO->bind();
}

void Water::bindRefractionFBO() 
{
	m_refractionFBO->bind();
}

const int res = 2;

FrameBufferObject const& Water::getReflectionFBO() 
{
	return *m_reflectionFBO;
}

void Water::drawVertices()
{
	glBindVertexArray(m_planeVAO);
	m_shader->use();
	glDrawElements(GL_TRIANGLES, (res - 1) * (res - 1) * 2 * 3, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Water::draw() 
{
	m_shader->use();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	SceneElements* se = DrawableObject::scene;

	this->setHeight(m_height);

	m_shader->setMat4("modelMatrix", m_modelMatrix);
	m_shader->setMat4("gVP", se->projMatrix * se->cam->GetViewMatrix());

	m_shader->setVec3("u_LightColor", se->lightColor);
	m_shader->setVec3("u_LightPosition", se->lightPos);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_reflectionFBO->tex);
	m_shader->setInt("reflectionTex", 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_refractionFBO->tex);
	m_shader->setInt("refractionTex", 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_dudvMap);
	m_shader->setInt("waterDUDV", 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_normalMap);
	m_shader->setInt("normalMap", 3);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, m_refractionFBO->depthTex);
	m_shader->setInt("depthMap", 4);

	float waveSpeed = 0.25;
	float time = glfwGetTime();

	float moveFactor = waveSpeed * time;
	m_shader->setFloat("moveFactor", moveFactor);

	m_shader->setVec3("cameraPosition", se->cam->Position);

	this->drawVertices();
	glDisable(GL_BLEND);
}

void Water::unbindFBO() 
{
	unbindCurrentFrameBuffer(Window::SCR_WIDTH, Window::SCR_WIDTH);
}

void Water::setPosition(glm::vec2 position, float scale, float height) 
{
	glm::mat4 identity;
	glm::mat4 scaleMatrix = glm::scale(identity, glm::vec3(scale, scale, scale));
	glm::mat4 transMatrix = glm::translate(identity, glm::vec3(position.x, height, position.y));
	m_modelMatrix = transMatrix * scaleMatrix;
}

void Water::setHeight(float height) 
{
	float scale = m_modelMatrix[0][0];
	float position_x = m_modelMatrix[3][0];
	float position_z = m_modelMatrix[3][2];

	glm::mat4 identity;
	glm::mat4 scaleMatrix = glm::scale(identity, glm::vec3(scale, scale, scale));
	glm::mat4 transMatrix = glm::translate(identity, glm::vec3(position_x, height, position_z));
	m_modelMatrix = transMatrix * scaleMatrix;
}

float Water::getHeight() 
{
	return m_height;
}

glm::mat4 Water::getModelMatrix() 
{
	return m_modelMatrix;
}
