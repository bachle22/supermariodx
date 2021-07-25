#pragma once

#include "GameObject.h"

constexpr float KOOPA_WALKING_SPEED = 0.03f;
constexpr float KOOPA_ROLLING_SPEED = 0.2f;
constexpr float KOOPA_FLYING_SPEED = 0.2f;
constexpr float KOOPA_PLATFORM_THRESHOLD = 0.05f;
constexpr float KOOPA_BOUNCE_SPEED = 0.1f;
constexpr float KOOPA_HIT_SPEED_X = 0.08f;
constexpr float KOOPA_HIT_SPEED_Y = 0.38f;
constexpr float KOOPA_HIT_MIN_SPEED = 0.002f;

constexpr int KOOPA_WIDTH = 14;
constexpr int KOOPA_WALKING_HEIGHT = 20;
constexpr int KOOPA_HIDING_HEIGHT = 15;

constexpr int KOOPA_WALKING_TRANS_Y = 6;
constexpr int KOOPA_SHELL_TRANS_X = 2;

constexpr int KOOPA_PLATFORM_OFFSET_LEFT = 6;
constexpr int KOOPA_PLATFORM_OFFSET_RIGHT = 12;

constexpr int KOOPA_HIDING_INTERVAL = 5000;
constexpr int KOOPA_REVIVING_INTERVAL = 2000;

constexpr int KOOPA_REFRESH_CONSTANT = 500;

enum KoopaAnimation
{
	KOOPA_GREEN_HIDING = 0,
	KOOPA_GREEN_WALKING = 1,
	KOOPA_GREEN_ROLLING = 2,
	KOOPA_GREEN_REVIVING = 3,
	KOOPA_GREEN_FLYING = 4,

	KOOPA_RED_HIDING = 5,
	KOOPA_RED_WALKING = 6,
	KOOPA_RED_ROLLING = 7,
	KOOPA_RED_REVIVING = 8,
	KOOPA_RED_FLYING = 9,
};

enum KoopaType
{
	KOOPA_GREEN = 0,
	KOOPA_RED = 1,
	PARAKOOPA_GREEN = 2,
	PARAKOOPA_RED = 3
};

enum KoopaState
{
	KOOPA_STATE_HIDING = 0,
	KOOPA_STATE_WALKING = 1,
	KOOPA_STATE_ROLLING = 2,
	KOOPA_STATE_REVIVING = 3,
	KOOPA_STATE_FLYING = 4
};

class Koopa : public GameObject
{
	int type;
	bool isFlipped, isHit;
	int bounce;
	float height;
	float leftBounding, rightBounding;
	ULONGLONG timer;
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	void GetPlatformBounding(LPGAMEOBJECT obj);

public:
	Koopa(int type);
	virtual void SetState(int state);
	void Reverse();
	void Downgrade();
	bool Hit();
	void Bounce();
};