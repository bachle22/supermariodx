#pragma once

#include "GameObject.h"

constexpr int PSWITCH_ANISET_ID = 73;
constexpr int PSWITCH_OFFSET_Y = 9;

enum PSwitchState
{
	PSWITCH_OFF = 0,
	PSWITCH_ON = 1,
};

class PSwitch : public GameObject
{
	int type;
	float entryX, entryY;

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();

public:
	PSwitch(float x, float y);
	virtual void SetState(int state);
	void Switch();
};