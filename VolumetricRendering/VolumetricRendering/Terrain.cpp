#include "Terrain.h"
#include "SceneElements.h"
#include <GLFW/glfw3.h>
#include "Tools.h"

bool Terrain::m_drawFog = true;

float sign(float x) 
{
	if (x > 0.0f) return 1.0f;
	else if (x < 0.0f) { return -1.0f; }
	else return 0.0f;
}


Terrain::Terrain(int gl)
{
	glm::mat4 id;
	glm::mat4 scaleMatrix = glm::scale(id, glm::vec3(1.0, 0.0, 1.0));
	glm::mat4 positionMatrix = glm::translate(id, glm::vec3(0., 0.0, 0.));
	m_modelMatrix = positionMatrix;

	m_octaves = 13;
	m_frequency = 0.01;
	m_grassCoverage = 0.65;
	m_tessMultiplier = 1.0;
	m_dispFactor = 20.0;

	m_fogFalloff = 1.5;

	m_posBuffer = 0;

	m_shader = new Shader("TerrainTessShader");
	m_shader->attachShader("shaders/terrain.vert")
			->attachShader("shaders/terrain.tcs")
			->attachShader("shaders/terrain.tes")
			->attachShader("shaders/terrain.frag")
			->linkPrograms();

	m_gridLength = gl + (gl + 1) % 2;


	m_res = 4;
	initializePlaneVAO(m_res, tileW, &m_planeVAO, &m_planeVBO, &m_planeEBO);

	m_textures = new unsigned int[6];
	m_textures[0] = TextureFromFile("sand.jpg", "../resources");
	m_textures[1] = TextureFromFile("grass.jpg", "../resources");
	m_textures[2] = TextureFromFile("rdiffuse.jpg", "../resources");
	m_textures[3] = TextureFromFile("snow2.jpg", "../resources");
	m_textures[4] = TextureFromFile("rnormal.jpg", "../resources");
	m_textures[5] = TextureFromFile("terrainTexture.jpg", "../resources");

	positionVec.resize(m_gridLength * m_gridLength);
	generateTileGrid(glm::vec2(0.0, 0.0));

	setPositionsArray(positionVec);

	m_rockColor = glm::vec4(120, 105, 75, 255) * 1.5f / 255.f;
	m_power = 3.0;
}

void Terrain::generateTileGrid(glm::vec2 offset)
{
	float sc = tileW;

	glm::vec2 I = glm::vec2(1, 0) * sc;
	glm::vec2 J = glm::vec2(0, 1) * sc;

	for (int i = 0; i < m_gridLength; i++) {
		for (int j = 0; j < m_gridLength; j++) {
			glm::vec2 pos = (float)(j - m_gridLength / 2) * glm::vec2(I) + (float)(i - m_gridLength / 2) * glm::vec2(J);
			setPos(i, j, pos + offset);
		}
	}
}

void Terrain::deleteBuffer() 
{
	glDeleteBuffers(1, &m_posBuffer);
	m_posBuffer = 0;
}

bool Terrain::getWhichTileCameraIs(glm::vec2& result) 
{
	for (glm::vec2 p : positionVec) 
	{
		if (inTile(*(scene->cam), p)) 
		{
			result = p;
			return true;
		}
	}
	return false;
}

void Terrain::getColRow(int i, int& col, int& row) 
{
	col = (i) % m_gridLength;
	row = (i - col) / m_gridLength;
}


void Terrain::draw() 
{
	SceneElements* se = DrawableObject::scene;

	m_drawFog = !se->wireframe;

	if (up != 0.0f) {
		glEnable(GL_CLIP_DISTANCE0);
	}
	glm::mat4 gWorld = m_modelMatrix;
	glm::mat4 gVP = se->projMatrix * se->cam->GetViewMatrix();

	m_shader->use();
	m_shader->setVec3("gEyeWorldPos", se->cam->Position);
	m_shader->setMat4("gWorld", gWorld);
	m_shader->setMat4("gVP", gVP);
	m_shader->setFloat("gDispFactor", m_dispFactor);

	float waterHeight = (waterPtr ? waterPtr->getModelMatrix()[3][1] : 100.0);
	glm::vec4 clipPlane(0.0, 1.0, 0.0, -waterHeight);
	m_shader->setVec4("clipPlane", clipPlane * up);
	m_shader->setVec3("u_LightColor", se->lightColor);
	m_shader->setVec3("u_LightPosition", se->lightPos);
	m_shader->setVec3("u_ViewPosition", se->cam->Position);
	m_shader->setVec3("fogColor", se->fogColor);
	m_shader->setVec3("rockColor", m_rockColor);
	m_shader->setVec3("seed", se->seed);

	m_shader->setInt("octaves", m_octaves);
	m_shader->setFloat("freq", m_frequency);
	m_shader->setFloat("u_grassCoverage", m_grassCoverage);
	m_shader->setFloat("waterHeight", waterHeight);
	m_shader->setFloat("tessMultiplier", m_tessMultiplier);
	m_shader->setFloat("fogFalloff", m_fogFalloff * 1.e-6);
	m_shader->setFloat("power", m_power);

	m_shader->setBool("normals", true);
	m_shader->setBool("drawFog", Terrain::m_drawFog);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_textures[0]);
	m_shader->setInt("sand", 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_textures[1]);
	m_shader->setInt("grass", 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_textures[2]);
	m_shader->setInt("rock", 3);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, m_textures[3]);
	m_shader->setInt("snow", 4);

	m_shader->setSampler2D("grass1", m_textures[5], 5);

	m_shader->setSampler2D("rockNormal", m_textures[4], 6);

	int nIstances = positionVec.size();

	drawVertices(nIstances);


	glDisable(GL_CLIP_DISTANCE0);
	up = 0.0;
}

void Terrain::drawVertices(int nInstances) 
{
	glBindVertexArray(m_planeVAO);
	m_shader->use();
	glDrawElementsInstanced(GL_PATCHES, (m_res - 1) * (m_res - 1) * 2 * 3, GL_UNSIGNED_INT, 0, nInstances);
	glBindVertexArray(0);
}

void Terrain::setPos(int row, int col, glm::vec2 pos) 
{
	positionVec[col + row * m_gridLength] = pos;
}

glm::vec2 Terrain::getPos(int row, int col) 
{
	return positionVec[col + row * m_gridLength];
}

void Terrain::setPositionsArray(std::vector<glm::vec2>& pos) 
{
	if (m_posBuffer) {
		this->deleteBuffer();
	}

	glGenBuffers(1, &m_posBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_posBuffer);
	glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(glm::vec2), &pos[0], GL_STATIC_DRAW);

	glBindVertexArray(m_planeVAO);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);

	glVertexAttribDivisor(3, 1);
	glBindVertexArray(0);

}

bool Terrain::inTile(const Camera camera, glm::vec2 pos) 
{
	float camX = camera.Position.x;
	float camY = camera.Position.z;

	float x = pos.x;
	float y = pos.y;

	bool inX = false;
	if ((camX <= x + 1.0 * tileW / 2.0f) && (camX >= x - 1.0 * tileW / 2.0f)) { inX = true; }
	bool inY = false;
	if ((camY <= y + 1.0 * tileW / 2.0f) && (camY >= y - 1.0 * tileW / 2.0f)) { inY = true; }

	bool result = inX && inY;

	return result;

}

void Terrain::setOctaves(int oct) 
{
	if (oct > 0) m_octaves = oct;
}

void Terrain::setFreq(float freq) 
{
	if (freq > 0.0f) m_frequency = freq;
}

void Terrain::setDispFactor(float disp) 
{
	if (disp > 0.0f) m_dispFactor = disp;
}

void Terrain::setScale(float scale) 
{
	glm::mat4 id;
	glm::mat4 scaleMatrix = glm::scale(id, glm::vec3(scale, 0.0, scale));
	glm::mat4 positionMatrix = glm::translate(id, glm::vec3(position.x * scale / m_scaleFactor, 0.0, position.y * scale / m_scaleFactor));
	m_modelMatrix = positionMatrix * scaleMatrix;
	m_scaleFactor = scale;
}

void Terrain::setGrassCoverage(float gc) 
{
	m_grassCoverage = gc;
}

void Terrain::setTessMultiplier(float tm) 
{
	if (tm > 0.0) m_tessMultiplier = tm;
}

int Terrain::getOctaves() const
{
	return m_octaves;
}

float Terrain::getFreq() const
{
	return m_frequency;
}

float Terrain::getDispFactor() const
{
	return m_dispFactor;
}

float Terrain::getScale() const
{
	return m_scaleFactor;
}

float Terrain::getGrassCoverage() const
{
	return m_grassCoverage;
}

float Terrain::getTessMultiplier() const
{
	return m_tessMultiplier;
}

void Terrain::updateTilesPositions() 
{
	SceneElements* se = DrawableObject::scene;
	glm::vec2 camPosition(se->cam->Position.x, se->cam->Position.z);
	int whichTile = -1;
	int howManyTiles = 0;

	glm::vec2 currentTile;
	if (getWhichTileCameraIs(currentTile)) {
		glm::vec2 center = getPos(m_gridLength / 2, m_gridLength / 2);
		for (glm::vec2& p : positionVec) {
			p += currentTile - center;
		}
		setPositionsArray(positionVec);

		if (waterPtr) {
			glm::vec2 center = getPos(m_gridLength / 2, m_gridLength / 2);
			waterPtr->setPosition(center, 1.0 * m_gridLength, waterPtr->getHeight());
		}
	}
}


void Terrain::reset() 
{
	int m_octaves = this->getOctaves();
	float freq = this->getFreq();
	float m_grassCoverage = this->getGrassCoverage();
	float dispFactor = this->getDispFactor();
	float tessMultiplier = this->getTessMultiplier();

	setOctaves(m_octaves);
	setFreq(freq);
	setGrassCoverage(m_grassCoverage);
	setDispFactor(dispFactor);
	setTessMultiplier(tessMultiplier);
}
