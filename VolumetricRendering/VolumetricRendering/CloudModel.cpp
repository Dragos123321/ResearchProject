#include "CloudModel.h"
#include <glad/glad.h>
#include "Texture.h"

CloudModel::CloudModel(SceneElements* scene, Skybox* sky) : m_scene{ scene }, m_sky{ sky }
{
	initVariables();
	initShaders();
	generateModelTextures();
}

void CloudModel::initShaders()
{
	m_volumetricCloudsShader = new Shader("volumetricCloudsShader", "shaders/volumetric_clouds.comp");
	m_postProcessingShader = new ScreenSpaceShader("shaders/clouds_post.frag");
	
	m_weatherShader = new Shader("weatherMap");
	m_weatherShader->attachShader("shaders/weather.comp");
	m_weatherShader->linkPrograms();
}

void CloudModel::generateModelTextures()
{
	if (!m_perlinTex) 
	{
		Shader comp("perlinWorley");
		comp.attachShader("shaders/perlinworley.comp");
		comp.linkPrograms();

		this->m_perlinTex = generateTexture3D(128, 128, 128);

		comp.use();
		comp.setVec3("u_resolution", glm::vec3(128, 128, 128));
		std::cout << "computing perlinworley!" << std::endl;
		glActiveTexture(GL_TEXTURE0);
		comp.setInt("outVolTex", 0);
		glBindTexture(GL_TEXTURE_3D, this->m_perlinTex);
		glBindImageTexture(0, this->m_perlinTex, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8);
		glDispatchCompute(INT_CEIL(128, 4), INT_CEIL(128, 4), INT_CEIL(128, 4));
		std::cout << "computed!!" << '\n';

		glGenerateMipmap(GL_TEXTURE_3D);
	}

	if (!m_worley32) {
		Shader worley_git("worleyComp");
		worley_git.attachShader("shaders/worley.comp");
		worley_git.linkPrograms();

		m_worley32 = generateTexture3D(32, 32, 32);

		worley_git.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, m_worley32);
		glBindImageTexture(0, m_worley32, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8);
		std::cout << "computing worley 32!" << std::endl;
		glDispatchCompute(INT_CEIL(32, 4), INT_CEIL(32, 4), INT_CEIL(32, 4));
		std::cout << "computed!!" << std::endl;
		glGenerateMipmap(GL_TEXTURE_3D);
	}

	if (!m_weatherTex) {
		m_weatherTex = generateTexture2D(1024, 1024);

		generateWeatherMap();

		m_seed = scene->seed;
		m_oldSeed = m_seed;
	}
}

CloudModel::~CloudModel()
{
	delete m_volumetricCloudsShader;
	delete m_postProcessingShader;
	delete m_weatherShader;
}

void CloudModel::update()
{
	m_seed = scene->seed;
	if (m_seed != m_oldSeed) {
		generateWeatherMap();
		m_oldSeed = m_seed;
	}
}

void CloudModel::generateWeatherMap() {
	bindTexture2D(m_weatherTex, 0);
	m_weatherShader->use();
	m_weatherShader->setVec3("seed", scene->seed);
	m_weatherShader->setFloat("perlinFrequency", m_perlinFrequency);
	std::cout << "computing weather!" << std::endl;
	glDispatchCompute(INT_CEIL(1024, 8), INT_CEIL(1024, 8), 1);
	std::cout << "weather computed!!" << std::endl;

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void CloudModel::initVariables()
{
	m_cloudSpeed = 450.0;
	m_coverage = 0.45;
	m_crispiness = 40.;
	m_curliness = .1;
	m_density = 0.02;
	m_absorption = 0.35;

	m_earthRadius = 600000.0;
	m_sphereInnerRadius = 5000.0;
	m_sphereOuterRadius = 17000.0;

	m_perlinFrequency = 0.8;

	m_enableGodRays = false;
	m_enablePowder = false;
	m_postProcess = true;

	m_seed = glm::vec3(0.0, 0.0, 0.0);
	m_oldSeed = glm::vec3(0.0, 0.0, 0.0);

	m_cloudColorTop = (glm::vec3(169., 149., 149.) * (1.5f / 255.f));
	m_cloudColorBottom = (glm::vec3(65., 70., 80.) * (1.5f / 255.f));

	m_weatherTex = 0;
	m_perlinTex = 0;
	m_worley32 = 0;
}
