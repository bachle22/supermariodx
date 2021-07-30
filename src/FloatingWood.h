#pragma once
#include "GameObject.h"

constexpr int FLOATING_WOOD_WIDTH = 48;
constexpr int FLOATING_WOOD_HEIGHT = 16;

constexpr float FLOATING_WOOD_SPEED = .03f;
constexpr float FLOATING_WOOD_GRAVITY = 0.0035f;

enum FloatingWoodState
{
	FLOATING_WOOD_MOVING = 0,
	FLOATING_WOOD_FALLING = 1
};

class FloatingWood : public GameObject
{
public:
	FloatingWood();
	bool isTouched;

	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	virtual void Render();
	virtual void Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects);
	virtual void SetState(int state);
};
