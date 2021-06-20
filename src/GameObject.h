#pragma once

#include <windows.h>
#include <d3dx9.h>
#include <vector>

#include "Animations.h"

class GameObject
{
protected:

	float x;
	float y;

	float vx;
	float vy;

	int nx;

	int state;

	DWORD dt;

	std::vector<LPANIMATION> animations;

public:
	void SetPosition(float x, float y) { this->x = x, this->y = y; }
	void SetSpeed(float vx, float vy) { this->vx = vx, this->vy = vy; }

	void SetState(int state) { this->state = state; }
	int GetState() { return this->state; }


	void AddAnimation(int aniId);

	GameObject();

	void Update(DWORD dt);
	void Render();
	~GameObject();
};