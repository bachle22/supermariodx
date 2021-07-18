#pragma once

#include "GameObject.h"

constexpr int PROJECTILE_ANI_SET_ID = 60;

constexpr int PROJECTILE_BBOX_WIDTH = 8;
constexpr int PROJECTILE_BBOX_HEIGH = 10;
constexpr float PROJECTILE_22DEG = 0.125f;
constexpr float PROJECTILE_90DEG = 0.5f;
constexpr float PROJECTILE_157DEG = 0.875f;


enum ProjectileType
{
	PROJECTILE_HIDDEN = 0,
	PROJECTILE_VISIBLE = 1,
};

enum ProjectileDirection
{
	UPPER_RIGHT = 0,
	TOP_RIGHT = 1,
	TOP_LEFT = 2,
	UPPER_LEFT = 3,

	LOWER_RIGHT = 4,
	BOTTOM_RIGHT = 5,
	BOTTOM_LEFT = 6,
	LOWER_LEFT = 7
};

class Projectile : public GameObject
{
	float entryY;

	virtual void Render();
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects);

public:
	Projectile(float x, float y, int direction);
	virtual void SetState(int state);

};