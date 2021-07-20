#pragma once

#include "GameObject.h"

constexpr int PLATFORM_UNPRESS_DELAY = 1000;

class Platform : public GameObject
{
	bool isPressed;
	ULONGLONG timer;
public:
	Platform();
	virtual void Render();
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects);
	void Press();
	void Unpress();
};