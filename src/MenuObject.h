#pragma once
#include "GameObject.h"

constexpr int OBJECT_TYPE_MENUOBJECT = 10;

constexpr int OBJECT_TYPE_BUSH = 1;
constexpr int OBJECT_TYPE_HAMMERBRO = 2;

constexpr float HAMMERBRO_SPEED = 0.015f;
constexpr int HAMMERBRO_RANGE = 12;


class MenuObject : public GameObject
{
	int type;
	float entryX{}, entryY{};

	virtual void Render();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	virtual void Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects);
public:
	MenuObject(int type);
	virtual void SetPosition(float x, float y);
};

