#pragma once

#include "GameObject.h"
#include "Coin.h"

constexpr float BRICK_ACCELERATION = -0.1f;
constexpr float BRICK_EMPTY_SHIFT = 0.01f;
constexpr int COIN_POSITION_OFFSET_X = 4;
constexpr int COIN_POSITION_OFFSET_Y = 2;
constexpr int MUSHROOM_OFFSET_X = 1;
constexpr int BRICK_UNPRESS_DELAY = 1000;

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

	float imitateY, entryY;

	bool isPressed;
	ULONGLONG timer;

public:
	Brick(int type);
	virtual void Render();
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects);
	virtual void SetPosition(float x, float y);

	void Hit();
	void Press();
	void Unpress();
};