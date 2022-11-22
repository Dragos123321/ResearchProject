#include "Skybox.h"

#include "sceneElements.h"
#include "Window.h"

Skybox::Skybox()
{
	m_skyColorTop = glm::vec3(0.5, 0.7, 0.8) * 1.05f;
	m_skyColorBottom = glm::vec3(0.9, 0.9, 0.95);

	m_skyboxShader = new ScreenSpaceShader("shaders/sky.frag");
	m_skyboxFBO = new FrameBufferObject(Window::SCR_WIDTH, Window::SCR_HEIGHT);

	SunsetPreset1();
	DefaultPreset();
}

ColorPreset Skybox::SunsetPreset() 
{
	ColorPreset preset;

	preset.cloudColorBottom = glm::vec3(89, 96, 109) / 255.f;
	preset.skyColorTop = glm::vec3(177, 174, 119) / 255.f;
	preset.skyColorBottom = glm::vec3(234, 125, 125) / 255.f;

	preset.lightColor = glm::vec3(255, 171, 125) / 255.f;
	preset.fogColor = glm::vec3(85, 97, 120) / 255.f;

	m_presetSunset = preset;

	return preset;
}

ColorPreset Skybox::SunsetPreset1() 
{
	ColorPreset preset;

	preset.cloudColorBottom = glm::vec3(97, 98, 120) / 255.f;
	preset.skyColorTop = glm::vec3(133, 158, 214) / 255.f;
	preset.skyColorBottom = glm::vec3(241, 161, 161) / 255.f;

	preset.lightColor = glm::vec3(255, 201, 201) / 255.f;
	preset.fogColor = glm::vec3(128, 153, 179) / 255.f;

	m_presetSunset = preset;

	return preset;
}

void Skybox::mixSkyColorPreset(float v, ColorPreset p1, ColorPreset p2)
{
	float a = std::min(std::max(v, 0.0f), 1.0f);
	float b = 1.0 - a;

	m_skyColorTop = p1.skyColorTop * a + p2.skyColorTop * b;
	m_skyColorBottom = p1.skyColorBottom * a + p2.skyColorBottom * b;
	scene->lightColor = p1.lightColor * a + p2.lightColor * b;
	scene->fogColor = p1.fogColor * a + p2.fogColor * b;
}


ColorPreset Skybox::DefaultPreset() 
{
	ColorPreset preset;

	preset.cloudColorBottom = (glm::vec3(65., 70., 80.) * (1.5f / 255.f));

	preset.skyColorTop = glm::vec3(0.5, 0.7, 0.8) * 1.05f;
	preset.skyColorBottom = glm::vec3(0.9, 0.9, 0.95);

	preset.lightColor = glm::vec3(255, 255, 230) / 255.f;
	preset.fogColor = glm::vec3(0.5, 0.6, 0.7);

	m_highSunPreset = preset;

	return preset;
}

unsigned int Skybox::getSkyTexture() 
{
	return m_skyboxFBO->tex;
}

void Skybox::draw() 
{
	SceneElements* s = DrawableObject::scene;
	m_skyboxFBO->bind();

	Shader& shader = m_skyboxShader->getShader();
	shader.use();

	shader.setVec2("resolution", glm::vec2(Window::SCR_WIDTH, Window::SCR_HEIGHT));
	shader.setMat4("inv_proj", glm::inverse(s->projMatrix));
	shader.setMat4("inv_view", glm::inverse(s->cam->GetViewMatrix()));

	shader.setVec3("lightDirection", glm::normalize(s->lightPos - s->cam->Position));

	shader.setVec3("skyColorTop", m_skyColorTop);
	shader.setVec3("skyColorBottom", m_skyColorBottom);

	m_skyboxShader->draw();
}

void Skybox::update() 
{
	auto sigmoid = [](float v) { return 1 / (1.0 + exp(8.0 - v * 40.0)); };
	mixSkyColorPreset(sigmoid(scene->lightDir.y), m_highSunPreset, m_presetSunset);
}

Skybox::~Skybox()
{
	delete m_skyboxFBO;
	delete m_skyboxShader;
}
