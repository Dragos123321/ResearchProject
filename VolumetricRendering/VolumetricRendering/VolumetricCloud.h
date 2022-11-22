#pragma once
#include "ScreenSpaceShader.h"
#include "Buffers.h"
#include "Texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <camera.h>
#include <algorithm>
#include "drawableObject.h"
#include "CloudModel.h"

class VolumetricCloud : public DrawableObject
{
public:
	VolumetricCloud(int SW, int SH, CloudModel* model);
	virtual void draw();
	~VolumetricCloud() = default;

	enum cloudsTextureNames { fragColor, bloom, alphaness, cloudDistance };

	unsigned int getCloudsTexture();

	unsigned int getCloudsTexture(int i);

	unsigned int getCloudsRawTexture();

private:
	int SCR_WIDTH;
	int SCR_HEIGHT;

	TextureSet* m_cloudsFBO;
	FrameBufferObject* m_cloudsPostProcessingFBO;

	CloudModel* m_model;
};

