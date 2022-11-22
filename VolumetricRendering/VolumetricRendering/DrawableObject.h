#pragma once

#include "sceneElements.h"

class DrawableObject
{
public:
	virtual void draw() = 0;
	virtual void update() {};

	static SceneElements* scene;
};