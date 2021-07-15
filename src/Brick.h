#pragma once

#include "GameObject.h"
#include "Coin.h"

constexpr float BRICK_GRAVITY = 0.003f;
constexpr float BRICK_ACCELERATION = -0.3f;
constexpr float BRICK_EMPTY_SHIFT = 0.01f;
constexpr int COIN_POSITION_OFFSET = 4;

enum BrickType
{
	BRICK_COIN = 1,
	BRICK_1UP = 2,
	BRICK_STARMAN = 3,
	BRICK_POWER_UP = 4,
	BRICK_BREAKABLE = 5,
	BRICK_PSWITCH = 6,
};

enum BrickState
{
	BRICK_STATE_DEFAULT = 0,
	BRICK_STATE_EMPTY = 1
};

enum BrickAnimation
{
	BRICK_ANI_DEFAULT = 0,
	BRICK_ANI_EMPTY = 1
};

class Brick : public GameObject
{
	int type;
	bool isHit;

	float entryY;

public:
	Brick(float x, float y, int type);
	virtual void Render();
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects);

	void Hit();
};