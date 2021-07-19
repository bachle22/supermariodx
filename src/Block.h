#pragma once

#include "GameObject.h"

class Block : public GameObject
{
	int width, height;

public:
	Block(int width, int height);
	virtual void Render();
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	float GetLeftX() { return x; }
	float GetRightX() { return x + width; }
};