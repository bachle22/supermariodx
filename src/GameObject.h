#pragma once

#include <windows.h>
#include <d3dx9.h>
#include <vector>

#include "Animations.h"

class GameObject;
typedef GameObject* LPGAMEOBJECT;

class GameObject
{
protected:

	float x;
	float y;

	float dx;	// dx = vx*dt
	float dy;	// dy = vy*dt

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

	virtual void Update(DWORD dt, std::vector<LPGAMEOBJECT>* coObjects = NULL);
	void Render();
	~GameObject();
};

