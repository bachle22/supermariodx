#pragma once

#include "GameObject.h"

constexpr float MUSHROOM_GROWTH_SPEED = 0.03f;
constexpr float MUSHROOM_MOVING_SPEED = 0.068f;
constexpr float MUSHROOM_GROWTH_HEIGHT = 16.1f;	// Prevent overlapping
constexpr float MUSHROOM_GRAVITY = 0.0015f;
constexpr int MUSHROOM_GROWING_DELAY = 200;

enum MushroomType
{
	SUPER_MUSHROOM = 40058,
	ONE_UP_MUSHROOM = 40060
};

enum MushroomState
{
	MUSHROOM_GROWING = 0,
	MUSHROOM_MOVING = 1,
};

class Mushroom : public GameObject
{
	int type;
	float entryX, entryY;
	LPSPRITE sprite;
	ULONGLONG growingDelay;

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();

public:
	Mushroom(float x, float y, int type);
	virtual void SetState(int state);
	void AddPoint();
};