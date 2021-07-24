#pragma once

#include "GameObject.h"

constexpr int BROKEN_BRICK_ANI_ID = 78;
constexpr float BROKEN_BRICK_ACCELERATION = 0.35f;
constexpr int BROKEN_BRICK_TIMEOUT = 800;

constexpr int BROKEN_BRICK_OFFSET_X = 18;
constexpr int BROKEN_BRICK_OFFSET_Y = 10;
constexpr int BROKEN_BRICK_WIDTH= 8;


class BrokenBrick : public GameObject
{
	bool isStarted;
	ULONGLONG timer;
	ULONGLONG frameStart, tickPerFrame;

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects = NULL);
	virtual void Render();

public:
	BrokenBrick(float x, float y);
	virtual void SetState(int state);
};
