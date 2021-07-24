#pragma once

#include "GameObject.h"

constexpr int ROULETTE_SWITCH_TIMEOUT = 100;

enum RouletteGoalType
{
	ROULETTE_UNKNOWN = 0,
	ROULETTE_STAR = 1,
	ROULETTE_FLOWER = 2,
	ROULETTE_MUSHROOM = 3,
};

enum RouletteState
{
	ROULETTE_STATE_SPINNING = 0,
	ROULETTE_STATE_WITHDRAWED = 1
};
class Roulette : public GameObject
{
	int type{};
	ULONGLONG timer;
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();

public:
	Roulette();
	virtual void SetState(int state);
	void Withdraw();
};