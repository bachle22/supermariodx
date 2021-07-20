#pragma once

#include "GameObject.h"

constexpr float KOOPA_WALKING_SPEED = 0.03f;

constexpr int KOOPA_WIDTH = 16;
constexpr int KOOPA_WALKING_HEIGHT = 26;
constexpr int KOOPA_PLATFORM_OFFSET_LEFT = 6;
constexpr int KOOPA_PLATFORM_OFFSET_RIGHT = 10;

#define KOOPA_BBOX_HEIGHT 26
#define KOOPA_BBOX_HEIGHT_DIE 16

#define KOOPA_STATE_DIE 200

#define KOOPA_ANI_WALKING_LEFT 0
#define KOOPA_ANI_WALKING_RIGHT 1
#define KOOPA_ANI_DIE 2

enum KoopaAnimation
{
	KOOPA_GREEN_WALKING = 0,
	KOOPA_GREEN_HIDING = 1,
	KOOPA_GREEN_ROLLING = 2,

	KOOPA_RED_WALKING = 3,
	KOOPA_RED_HIDING = 4,
	KOOPA_RED_ROLLING = 5,

	KOOPA_GREEN_REVIVING = 6,
	KOOPA_RED_REVIVING = 7,
};

enum KoopaType
{
	KOOPA_GREEN = 0,
	KOOPA_RED = 1
};

enum KoopaState
{
	KOOPA_STATE_WALKING = 0,
};

class Koopa : public GameObject
{
	int type;
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
};