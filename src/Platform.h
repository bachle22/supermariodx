#pragma once

#include "GameObject.h"

class Platform : public GameObject
{
public:
	virtual void Render();
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
};