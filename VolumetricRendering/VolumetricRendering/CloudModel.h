#pragma once
#include <glm/glm.hpp>
#include "Shader.h"
#include "sceneElements.h"
#include "ScreenSpaceShader.h"
#include "Skybox.h"

#define INT_CEIL(n,d) (int)ceil((float)n/d)

class CloudModel : public DrawableObject
{
public:
	friend class VolumetricCloud;

	CloudModel(SceneElements* scene, Skybox* sky);
	~CloudModel();

	virtual void draw() {};

	virtual void update();

private:	
	void generateWeatherMap();
	void generateModelTextures();
	void initVariables();
	void initShaders();

	Shader* m_volumetricCloudsShader;
	Shader* m_weatherShader;
	ScreenSpaceShader* m_postProcessingShader;

	float m_coverage;
	float m_cloudSpeed;
	float m_crispiness;
	float m_curliness;
	float m_density;
	float m_absorption;
	float m_earthRadius;
	float m_sphereInnerRadius;
	float m_sphereOuterRadius;
	float m_perlinFrequency;
	bool m_enableGodRays;
	bool m_enablePowder;
	bool m_postProcess;

	glm::vec3 m_cloudColorTop;
	glm::vec3 m_cloudColorBottom;

	glm::vec3 m_seed; 
	glm::vec3 m_oldSeed;
	unsigned int m_perlinTex;
	unsigned int m_worley32;
	unsigned int m_weatherTex;

	SceneElements* m_scene;
	Skybox* m_sky;
};