#pragma once

#include "GameObject.h"

enum BrickType
{
	BRICK_ITEM_COIN = 1,
	BRICK_ITEM_EXTRA_LIFE = 2,
	BRICK_STARMAN = 3,
	BRICK_POWER_UP = 4,
	BRICK_BREAKABLE = 5,
	BRICK_PSWITCH = 6,
	BRICK_FIREFLOWER = 7
};

class Brick : public GameObject
{
	int type;
	bool isHit;
	ULONGLONG timer;

	float temp_y;

public:
	Brick(float x, float y, int type);
	virtual void Render();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);

	virtual void Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects);

	void Hit();
};