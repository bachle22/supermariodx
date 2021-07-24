#pragma once
#include "GameObject.h"

constexpr int HIT_ANIMATION_DURATION = 200;
constexpr int HIT_ANIMSET_ID = 75;

class Hit : public GameObject
{
	ULONGLONG timer;
	ULONGLONG frameStart, tickPerFrame;

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects = NULL);
	virtual void Render();

public:
	Hit(float x, float y);
	virtual void SetState(int state);

};
