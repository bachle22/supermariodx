#pragma once

#include "GameObject.h"

constexpr float POINT_FLOATING_SPEED = 0.12f;
constexpr float POINT_MIN_SPEED = -0.01f;
constexpr int POINT_FLOATING_HEIGHT = 30;

enum PointType
{
	POINT_100 = 50049,
	POINT_200 = 50050,
	POINT_400 = 50051,
	POINT_800 = 50052,
	POINT_1000 = 50053,
	POINT_2000 = 50054,
	POINT_4000 = 50055,
	POINT_8000 = 50056,
	POINT_1UP = 50057,
};

enum PointScore {
	SCORE_100 = 100,
	SCORE_200 = 200,
	SCORE_400 = 400,
	SCORE_800 = 800,
	SCORE_1000 = 1000,
	SCORE_2000 = 2000,
	SCORE_4000 = 4000,
	SCORE_8000 = 8000,
	SCORE_1UP = 100,
};


class Point : public GameObject
{
	int type;
	float entryY;
	LPSPRITE point;

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects = NULL);
	virtual void Render();

public:
	Point(float x, float y, int type);
	virtual void SetState(int state);

};
