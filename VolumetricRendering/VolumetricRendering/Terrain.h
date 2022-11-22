#pragma once
#include <glm/glm.hpp>
//#include <model.h>
#include "Texture.h"
#include "Water.h"
#include <camera.h>
#include "GlError.h"
#include "drawableObject.h"
#include "Water.h"

enum tPosition 
{
	C, N, S, E, W, SE, SW, NE, NW, totTiles
};

class Terrain : public DrawableObject
{
public:
	Terrain(int gl);

	virtual ~Terrain() = default;
	virtual void draw();

	void updateTilesPositions();
	void setPositionsArray(std::vector<glm::vec2>& pos);

	bool inTile(Camera camera, glm::vec2 pos);

	void setOctaves(int oct);

	void setFreq(float freq);

	void setDispFactor(float disp);

	void setScale(float scale);

	void setGrassCoverage(float gc);

	void setTessMultiplier(float tm);

	int getOctaves() const;
	float getFreq() const;
	float getDispFactor() const;
	float getScale() const;
	float getGrassCoverage() const;
	float getTessMultiplier() const;

	glm::vec2 position;
	glm::vec2 eps;
	float up = 0.0;
	static const int tileW = 10. * 100.;
	Water* waterPtr;

private:
	void deleteBuffer();
	void drawVertices(int nInstances);
	void setPos(int row, int col, glm::vec2 pos);
	glm::vec2 getPos(int row, int col);
	void generateTileGrid(glm::vec2 offset);
	bool getWhichTileCameraIs(glm::vec2& result);
	void getColRow(int i, int& col, int& row);
	void reset();

	int m_res;
	unsigned int m_planeVBO;
	unsigned int m_planeVAO;
	unsigned int m_planeEBO;
	float m_dispFactor;
	float m_scaleFactor;
	float m_frequency;
	float m_grassCoverage;
	float m_tessMultiplier;
	float m_fogFalloff;
	float m_power;
	int m_octaves;
	int m_gridLength;
	glm::vec3 m_rockColor;

	static bool m_drawFog;
	unsigned int* m_textures;
	unsigned int m_posBuffer;

	Shader* m_shader;
	glm::mat4 m_modelMatrix;

	std::vector<glm::vec2> positionVec;
};

