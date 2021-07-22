#pragma once
#include "GameObject.h"

constexpr float GOOMBA_WALKING_SPEED = 0.03f;
constexpr int GOOMBA_REMOVAL_DELAY = 300;

constexpr float GOOMBA_SMACKED_VX = 0.03f;
constexpr float GOOMBA_SMACKED_VY = 0.38f;

constexpr float PARAGOOMBA_SKIP_VY = 0.08f;
constexpr float PARAGOOMBA_JUMP_VY = 0.2f;

constexpr int GOOMBA_BBOX_WIDTH = 16;
constexpr int GOOMBA_BBOX_HEIGHT = 15;
constexpr int GOOMBA_BBOX_HEIGHT_DIE = 9;

constexpr int PARAGOOMBA_TRANS_X = 2;
constexpr int PARAGOOMBA_TRANS_Y = 8;

constexpr int PARAGOOMBA_JUMP_INTERVAL = 1000;
constexpr int PARAGOOMBA_JUMP_DELAY = 2500;


enum GoombaState
{
	GOOMBA_STATE_DIE = 0,
	GOOMBA_STATE_WALKING = 1,
	GOOMBA_STATE_SKIPPING = 2,
	GOOMBA_STATE_HIT = 3
};

enum GoombaAnimation
{
	GOOMBA_ANI_DIE = 0,
	GOOMBA_ANI_WALKING = 1,
	PARAGOOMBA_ANI_WALKING = 2,
	PARAGOOMBA_ANI_SKIPPING = 3,
};

enum GoombaType
{
	GOOMBA_BROWN = 0,
	GOOMBA_RED = 1
};


class Goomba : public GameObject
{
	int type;
	ULONGLONG timer;
	int skippingCounter;
	

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();

public:
	Goomba(int type);
	virtual void SetState(int state);
	bool Hit();
	void Reverse();
	void Skip();
	void Downgrade();
};