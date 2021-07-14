#pragma once

#include "GameObject.h"

class Block : public GameObject
{
	float width, height;

public:
	Block(float width, float height);
	virtual void Render();
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
};