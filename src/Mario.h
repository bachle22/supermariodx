#pragma once

#include "GameObject.h"

#define MARIO_WIDTH 14

class Mario : public GameObject
{
protected:
	float vx;
public:
	Mario(float x, float y, float vx);
	void Update(DWORD dt);
	void Render();
};
