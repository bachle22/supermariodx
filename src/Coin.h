#pragma once

#include "GameObject.h"

constexpr float COIN_GRAVITY = 0.001f;
constexpr float COIN_VELOCITY = -.35f;
constexpr float COIN_FALLING_FLOOR = 20;
constexpr int COIN_ANI_SET_ID = 70;


enum CoinBBox
{
	COIN_BBOX_WIDTH = 14,
	COIN_BBOX_HEIGH = 16
};

enum CoinType
{
	COIN_HIDDEN = 0,
	COIN_VISIBLE = 1,
};

class Coin : public GameObject
{
	bool isThrowing;
	int type;
	float entryY;
	
	virtual void Render();
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects);

public:
	Coin(float x, float y, int state);
	void Throw();

};