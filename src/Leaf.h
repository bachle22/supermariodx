#pragma once
#include "GameObject.h"

constexpr int LEAF_SPRITE_ID = 40057;
constexpr int LEAF_GROWING_DELAY = 200;
constexpr int LEAF_WIDTH = 16;
constexpr int LEAF_HEIGHT = 14;
constexpr int LEAF_MAX_OFFSET = 16;

constexpr float LEAF_FLY_SPEED = 0.2f;
constexpr float LEAF_MOVING_SPEED = 0.06f;
constexpr float LEAF_GROWTH_HEIGHT = 32;
constexpr float LEAF_TERMINAL_VELOCITY = GLOBAL_TERMINAL_VELOCITY / 5;
constexpr float LEAF_ACCELERATION_Y = 0.018f;

enum LeafState
{
	LEAF_GROWING = 0,
	LEAF_MOVING = 1,
};

class Leaf : public GameObject
{
	float entryX, entryY;
	LPSPRITE sprite;
	ULONGLONG growingDelay;

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();

public:
	Leaf(float x, float y);
	virtual void SetState(int state);
	void Consume();
};