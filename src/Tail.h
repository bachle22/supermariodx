#pragma once
#include "GameObject.h"
#include "Hit.h"

constexpr float TAIL_OFFSET_LEFT = 6;
constexpr float TAIL_OFFSET_RIGHT = 3;
constexpr float TAIL_OFFSET_Y = 18;
constexpr float TAIL_VELOCITY = 0.1f;
constexpr int TAIL_HEIGHT = 6;

class Tail : public GameObject
{
	ULONGLONG timer;

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();

public:
	Tail();
	virtual void SetPosition(float x, float y);
};