#pragma once

#include "GameObject.h"

constexpr int WARP_ANISET_ID = 79;
constexpr int WARP_TIMEOUT = 500;


class Warp : public GameObject
{
	ULONGLONG timer;
	ULONGLONG frameStart, tickPerFrame;

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects = NULL);
	virtual void Render();

public:
	Warp(float x, float y);
	virtual void SetState(int state);
};
