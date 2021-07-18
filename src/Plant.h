#pragma once
#include "GameObject.h"

constexpr float PLANT_WALKING_SPEED = 0.03f;
constexpr int PLANT_REMOVAL_DELAY = 300;
constexpr int PLANT_BBOX_WIDTH = 16;
constexpr int PLANT_BBOX_HEIGHT = 32;
constexpr int PLANT_RED_BBOX_HEIGHT = 32;

enum PlantType
{
	PLANT_GREEN_BITER = 0,
	PLANT_RED_SHOOTER = 1,
	PLANT_GREEN_SHOOTER = 2,
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
	PLANT_GREEN_UP = 0,

	PLANT_RED_ANI_SHOOTING_UP = 1,
	PLANT_RED_ANI_SHOOTING_DOWN = 2,
	PLANT_RED_ANI_BITING_UP = 3,
	PLANT_RED_ANI_BITING_DOWN = 4,
};


class Plant : public GameObject
{
	int type;
	int ny;
	int height;
	float entryY;
	ULONGLONG timer;

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();

public:
	Plant(int type);
	virtual void SetState(int state);
	virtual void SetPosition(float x, float y);
};