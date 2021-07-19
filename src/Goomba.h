#pragma once
#include "GameObject.h"

constexpr float GOOMBA_WALKING_SPEED = 0.03f;
constexpr int GOOMBA_REMOVAL_DELAY = 300;

constexpr float GOOMBA_SMACKED_VX = 0.03f;
constexpr float GOOMBA_SMACKED_VY = 0.38f;

constexpr int GOOMBA_BBOX_WIDTH = 16;
constexpr int GOOMBA_BBOX_HEIGHT = 15;
constexpr int GOOMBA_BBOX_HEIGHT_DIE = 9;

enum GoombaState
{
	GOOMBA_STATE_WALKING = 0,
	GOOMBA_STATE_DIE = 1,
	GOOMBA_STATE_HIT = 2
};

enum GoombaAnimation
{
	GOOMBA_ANI_WALKING = 0,
	GOOMBA_ANI_DIE = 1
};


class Goomba : public GameObject
{
	ULONGLONG timer;

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();

public:
	Goomba();
	virtual void SetState(int state);
	bool Hit();
	void Reverse();
};