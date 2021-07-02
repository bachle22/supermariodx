#pragma once

#include "GameObject.h"


constexpr float MARIO_WALKING_SPEED = 0.12f;
constexpr float MARIO_WALKING_SPEED_SMALL = 0.095f;
constexpr float MARIO_JUMP_SPEED_LOW = 0.225f;
constexpr float MARIO_JUMP_SPEED_HIGH = 0.15f;
constexpr float MARIO_JUMP_HEIGHT_MAX = 50.0f;
constexpr float MARIO_JUMP_DEFLECT_SPEED = 0.2f;
constexpr float MARIO_GRAVITY = 0.001f;
constexpr float MARIO_DIE_DEFLECT_SPEED = 0.5f;

constexpr int MARIO_STATE_IDLE = 0;
constexpr int MARIO_STATE_WALKING_RIGHT = 100;
constexpr int MARIO_STATE_WALKING_LEFT = 200;
constexpr int MARIO_STATE_JUMP = 300;
constexpr int MARIO_STATE_JUMP_LOW = 301;
constexpr int MARIO_STATE_JUMP_HIGH = 302;
constexpr int MARIO_STATE_DIE = 400;

constexpr int MARIO_ANI_SMALL_IDLE = 0;
constexpr int MARIO_ANI_BIG_IDLE = 1;

constexpr int MARIO_ANI_SMALL_WALKING = 4;
constexpr int MARIO_ANI_BIG_WALKING = 5;


constexpr int MARIO_ANI_SMALL_JUMPING = 8;
constexpr int MARIO_ANI_BIG_JUMPING = 9;
constexpr int MARIO_ANI_BIG_JUMPED = 62;

constexpr int MARIO_ANI_DIE = 8;

constexpr int MARIO_ANI_BIG_DUCKING = 40;

constexpr int MARIO_LEVEL_SMALL = 1;
constexpr int MARIO_LEVEL_BIG = 2;

constexpr int MARIO_BIG_BBOX_WIDTH = 16;
constexpr int MARIO_BIG_BBOX_HEIGHT = 27;

constexpr int MARIO_SMALL_BBOX_WIDTH = 14;
constexpr int MARIO_SMALL_BBOX_HEIGHT = 16;

constexpr int MARIO_UNTOUCHABLE_TIME = 5000;

constexpr float MARIO_ACCELERATION_X = 0.03f;
constexpr float MARIO_ACCELERATION_Y = 0.0262f;
constexpr float MARIO_INERTIA = 0.028f;
constexpr float MARIO_INERTIA_SMALL = 0.028f;

//constexpr int LEFT = 0;
//constexpr int RIGHT = 1;
//constexpr int UP = 2;
//constexpr int DOWN = 3;

enum Direction {
	LEFT = 0,
	RIGHT = 1,
	UP = 2,
	DOWN = 3,
};

class Mario : public GameObject
{
	int level;
	int untouchable;
	ULONGLONG untouchable_start;

	float start_x;
	float start_y;

	float ax, ay;

	bool isTurbo;
	bool isJumping, canJumpAgain;
	float last_y;
	bool edges[4] = { 0, 0, 0, 0 };
	bool movement[4] = { 0, 0, 0, 0 };

	ULONGLONG jump_time_start, jump_time_end;



public:
	Mario(float x = 0.0f, float y = 0.0f);
	virtual void Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* colliable_objects = NULL);
	virtual void Render();

	void IsJumping(bool value) { isJumping = value; }
	bool IsJumping() { return isJumping; }

	void CanJumpAgain(bool value) { canJumpAgain = value; }
	bool CanJumpAgain() { return canJumpAgain; }

	void SetTurbo(bool value) { isTurbo = value; }

	void Movement();

	void SetMovement(int direction) { movement[direction] = true; };
	void UnsetMovement(int direction) { movement[direction] = false; };

	void SetState(int state);
	void SetLevel(int l) { level = l; }
	void StartUntouchable() { untouchable = 1; untouchable_start = GetTickCount64(); }
	void Reset();
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
};