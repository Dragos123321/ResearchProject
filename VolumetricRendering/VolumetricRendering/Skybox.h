#pragma once

#include <glm/glm.hpp>
#include "ScreenSpaceShader.h"
#include "DrawableObject.h"
#include <glm/gtc/matrix_transform.hpp>

struct ColorPreset {
	glm::vec3 cloudColorBottom, skyColorTop, skyColorBottom, lightColor, fogColor;
};

class Skybox : public DrawableObject {
public:
	friend class VolumetricCloud;
	Skybox();
	~Skybox();

	virtual void draw();
	virtual void update();

	ColorPreset DefaultPreset();
	ColorPreset SunsetPreset();
	ColorPreset SunsetPreset1();

	void mixSkyColorPreset(float v, ColorPreset p1, ColorPreset p2);

	unsigned int getSkyTexture();

private:
	glm::vec3 m_skyColorTop;
	glm::vec3 m_skyColorBottom;

	ScreenSpaceShader* m_skyboxShader;
	FrameBufferObject* m_skyboxFBO;
	
	ColorPreset m_presetSunset;
	ColorPreset m_highSunPreset;
};