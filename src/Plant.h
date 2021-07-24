#pragma once
#include "GameObject.h"

constexpr float PLANT_WALKING_SPEED = 0.03f;
constexpr int PLANT_REMOVAL_DELAY = 300;
constexpr int PLANT_WIDTH = 16;
constexpr int PLANT_RED_HEIGHT = 32;
constexpr int PLANT_GREEN_HEIGHT = 24;

constexpr int PLANT_PROJECTILE_OFFSET_XX = 12;
constexpr int PLANT_PROJECTILE_OFFSET_XY = 5;
constexpr int PLANT_PROJECTILE_OFFSET_YX = 6;
constexpr int PLANT_PROJECTILE_OFFSET_YY = 2;

constexpr float PLANT_VELOCITY_Y = 0.045f;

constexpr int PLANT_BITER_HIDING_INTERVAL = 750;
constexpr int PLANT_BITER_ATTACKING_INTERVAL = 750;
constexpr int PLANT_SHOOTER_HIDING_INTERVAL = 1500;
constexpr int PLANT_PROJECTILE_DELAY = 1000;
constexpr int PLANT_SHOOTER_ATTACKING_INTERVAL = 1500;

constexpr int PLANT_WARP_EFFECT_Y = 4;

enum PlantType
{
	PLANT_GREEN_BITER = 0,
	PLANT_GREEN_SHOOTER = 1,
	PLANT_RED_SHOOTER = 2,
};

enum PlantState
{
	PLANT_STATE_HIDING = 0,
	PLANT_STATE_EXPANDING = 1,
	PLANT_STATE_ATTACKING = 2,
	PLANT_STATE_COLLAPSING = 3,
	PLANT_STATE_DEAD = 4,
};

enum PlantAnimation
{
	PLANT_GREEN_ANI = 0,

	PLANT_RED_ANI_SHOOTING_UP = 1,
	PLANT_RED_ANI_SHOOTING_DOWN = 2,
	PLANT_RED_ANI_BITING_UP = 3,
	PLANT_RED_ANI_BITING_DOWN = 4,

	PLANT_GREEN_ANI_SHOOTING_UP = 5,
	PLANT_GREEN_ANI_SHOOTING_DOWN = 6,
	PLANT_GREEN_ANI_BITING_UP = 7,
	PLANT_GREEN_ANI_BITING_DOWN = 8,
};

class Plant : public GameObject
{
	int type;
	int ny;
	int height;
	float entryY;
	bool isProjectileShooted;
	ULONGLONG timer;

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();

public:
	Plant(int type);
	virtual void SetState(int state);
	virtual void SetPosition(float x, float y);
	bool Hit();
};