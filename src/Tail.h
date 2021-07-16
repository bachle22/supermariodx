#pragma once
#include "GameObject.h"

constexpr float TAIL_OFFSET_X = 2;
constexpr float TAIL_OFFSET_Y = 12;
constexpr float TAIL_VELOCITY = 0.1f;

class Tail : public GameObject
{
	ULONGLONG timer;

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();

public:
	Tail();
	virtual void SetPosition(float x, float y);
	void SetDirection(int nx) { this->nx = nx; }
};