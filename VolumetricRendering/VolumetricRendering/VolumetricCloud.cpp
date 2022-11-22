#include "VolumetricCloud.h"

#define TIMETO(CODE, TASK) 	t1 = glfwGetTime(); CODE; t2 = glfwGetTime(); std::cout << "Time to " + std::string(TASK) + " :" << (t2 - t1)*1e3 << "ms" << std::endl;

VolumetricCloud::VolumetricCloud(int SW, int SH, CloudModel* m_model) : SCR_WIDTH{ SW }, SCR_HEIGHT{ SH }, m_model{ m_model } 
{
	m_cloudsFBO = new TextureSet(SW, SH, 4);
	m_cloudsPostProcessingFBO = new FrameBufferObject(Window::SCR_WIDTH, Window::SCR_HEIGHT, 2);
}


void VolumetricCloud::draw() 
{
	float t1, t2;

	for (int i = 0; i < m_cloudsFBO->getNTextures(); ++i) 
	{
		bindTexture2D(m_cloudsFBO->getColorAttachmentTex(i), i);
	}

	Shader& cloudsShader = *m_model->m_volumetricCloudsShader;
	SceneElements* s = DrawableObject::scene;

	cloudsShader.use();

	cloudsShader.setVec2("iResolution", glm::vec2(SCR_WIDTH, SCR_HEIGHT));
	cloudsShader.setFloat("iTime", glfwGetTime());
	cloudsShader.setMat4("inv_proj", glm::inverse(s->projMatrix));
	cloudsShader.setMat4("inv_view", glm::inverse(s->cam->GetViewMatrix()));
	cloudsShader.setVec3("cameraPosition", s->cam->Position);
	cloudsShader.setFloat("FOV", s->cam->Zoom);
	cloudsShader.setVec3("lightDirection", glm::normalize(s->lightPos - s->cam->Position));
	cloudsShader.setVec3("lightColor", s->lightColor);

	cloudsShader.setFloat("coverage_multiplier", m_model->m_coverage);
	cloudsShader.setFloat("cloudSpeed", m_model->m_cloudSpeed);
	cloudsShader.setFloat("crispiness", m_model->m_crispiness);
	cloudsShader.setFloat("curliness", m_model->m_curliness);
	cloudsShader.setFloat("absorption", m_model->m_absorption * 0.01);
	cloudsShader.setFloat("densityFactor", m_model->m_density);

	cloudsShader.setFloat("earthRadius", m_model->m_earthRadius);
	cloudsShader.setFloat("sphereInnerRadius", m_model->m_sphereInnerRadius);
	cloudsShader.setFloat("sphereOuterRadius", m_model->m_sphereOuterRadius);

	cloudsShader.setVec3("cloudColorTop", m_model->m_cloudColorTop);
	cloudsShader.setVec3("cloudColorBottom", m_model->m_cloudColorBottom);

	cloudsShader.setVec3("skyColorTop", m_model->m_sky->m_skyColorTop);
	cloudsShader.setVec3("skyColorBottom", m_model->m_sky->m_skyColorBottom);

	glm::mat4 vp = s->projMatrix * s->cam->GetViewMatrix();
	cloudsShader.setMat4("invViewProj", glm::inverse(vp));
	cloudsShader.setMat4("gVP", vp);

	cloudsShader.setSampler3D("cloud", m_model->m_perlinTex, 0);
	cloudsShader.setSampler3D("worley32", m_model->m_worley32, 1);
	cloudsShader.setSampler2D("weatherTex", m_model->m_weatherTex, 2);
	cloudsShader.setSampler2D("depthMap", s->sceneFBO->depthTex, 3);

	cloudsShader.setSampler2D("sky", m_model->m_sky->getSkyTexture(), 4);

	if (!s->wireframe)
		glDispatchCompute(INT_CEIL(SCR_WIDTH, 16), INT_CEIL(SCR_HEIGHT, 16), 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	if (m_model->m_postProcess) 
	{
		m_cloudsPostProcessingFBO->bind();
		Shader& cloudsPPShader = m_model->m_postProcessingShader->getShader();

		cloudsPPShader.use();

		cloudsPPShader.setSampler2D("clouds", m_cloudsFBO->getColorAttachmentTex(VolumetricCloud::fragColor), 0);
		cloudsPPShader.setSampler2D("emissions", m_cloudsFBO->getColorAttachmentTex(VolumetricCloud::bloom), 1);
		cloudsPPShader.setSampler2D("depthMap", s->sceneFBO->depthTex, 2);

		cloudsPPShader.setVec2("cloudRenderResolution", glm::vec2(SCR_WIDTH, SCR_HEIGHT));
		cloudsPPShader.setVec2("resolution", glm::vec2(Window::SCR_WIDTH, Window::SCR_HEIGHT));

		glm::mat4 lightModel;
		lightModel = glm::translate(lightModel, s->lightPos);
		glm::vec4 pos = vp * lightModel * glm::vec4(0.0, 60.0, 0.0, 1.0);
		pos = pos / pos.w;
		pos = pos * 0.5f + 0.5f;

		cloudsPPShader.setVec4("lightPos", pos);

		bool isLightInFront = false;
		float lightDotCameraFront = glm::dot(glm::normalize(s->lightPos - s->cam->Position), glm::normalize(s->cam->Front));
		if (lightDotCameraFront > 0.2) {
			isLightInFront = true;
		}

		cloudsPPShader.setBool("isLightInFront", isLightInFront);
		cloudsPPShader.setBool("enableGodRays", m_model->m_enableGodRays);
		cloudsPPShader.setFloat("lightDotCameraFront", lightDotCameraFront);

		cloudsPPShader.setFloat("time", glfwGetTime());
		if (!s->wireframe)
			m_model->m_postProcessingShader->draw();
	}
}

unsigned int VolumetricCloud::getCloudsTexture() 
{
	return (m_model->m_postProcess ? m_cloudsPostProcessingFBO->getColorAttachmentTex(0) : getCloudsRawTexture());
}

unsigned int VolumetricCloud::getCloudsTexture(int i) 
{
	return m_cloudsFBO->getColorAttachmentTex(i);
}

unsigned int VolumetricCloud::getCloudsRawTexture() 
{
	return m_cloudsFBO->getColorAttachmentTex(0);
}
