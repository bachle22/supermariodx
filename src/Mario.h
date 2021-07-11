#pragma once

#include "GameObject.h"

constexpr int MARIO_STATE_IDLE = 0;
constexpr int MARIO_STATE_WALKING_RIGHT = 100;
constexpr int MARIO_STATE_WALKING_LEFT = 200;
constexpr int MARIO_STATE_JUMP = 300;
constexpr int MARIO_STATE_JUMP_LOW = 301;
constexpr int MARIO_STATE_JUMP_HIGH = 302;
constexpr int MARIO_STATE_DIE = 400;

constexpr int MARIO_UNTOUCHABLE_TIME = 3000;

constexpr float MARIO_WALKING_SPEED = 0.11f;
constexpr float MARIO_WALKING_SPEED_SMALL = 0.09f;
constexpr float MARIO_JUMP_SPEED_LOW = 0.25f;
constexpr float MARIO_JUMP_SPEED_HIGH = 0.15f;
constexpr float MARIO_JUMP_HEIGHT_MAX = 50.0f;
constexpr float MARIO_JUMP_HEIGHT_POWER = 2.0f;
constexpr float MARIO_JUMP_DEFLECT_SPEED = 0.2f;
constexpr float MARIO_GRAVITY = 0.001f;
constexpr float MARIO_DIE_DEFLECT_SPEED = 0.5f;
constexpr float MARIO_DESCENDING_SPEED = 0.06f;

constexpr float MARIO_ACCELERATION_X = 0.03f;
constexpr float MARIO_ACCELERATION_Y = 0.001f;
constexpr float MARIO_INERTIA = 0.028f;
constexpr float MARIO_INERTIA_SMALL = 0.018f;
constexpr float MARIO_POWER_ACCELERATION = .015f;
constexpr float MARIO_POWER_JUMP = 0.1f;
constexpr float MARIO_POWER_INERTIA = 0.003f;
constexpr float MARIO_FLY_ACCELERATION_X = 0.96f;
constexpr float MARIO_FLYING_GRAVITY = 0.02f;
constexpr float MARIO_FLY_SPEED = 0.0001f;
constexpr float MARIO_BRAKE_INERTIA = .01f;
constexpr float MARIO_BRAKE_IDLE_INERTIA = .08f;
constexpr float MARIO_VELOCITY_Y_THRESHOLD = -0.08f;

constexpr float POWER_ALLOW_GAINING_THRESHOLD = .3f;

constexpr int POWER_MAX_FLY_TIME = 3500;
constexpr int POWER_UP_DURATION_STEP = 300;
constexpr int POWER_DOWN_DURATION_STEP = 400;
constexpr int POWER_PEAKED_DECREASE_TIME = 80;
constexpr int POWER_DIRECTION_UNCHANGED_STEP = 350;
constexpr int POWER_DIRECTION_CHANGED_STEP = 200;

constexpr int MAX_POWER_METER = 7;

constexpr int BIG_BBOX_DUCKING_DIFF = 9;
constexpr int RACOON_BBOX_DUCKING_DIFF = 10;

enum MarioBBox
{
	SMALL_WIDTH = 14,
	SMALL_HEIGHT = 16,
	BIG_WIDTH = 15,
	BIG_HEIGHT = 27,
	RACOON_WIDTH = 23,
	RACOON_HEIGHT = 28,
	DUCKING_HEIGHT = 18,
};

enum MarioAnimation
{
	SMALL_IDLE = 0,
	BIG_IDLE = 1,
	RACOON_IDLE = 3,

	SMALL_WALKING = 4,
	BIG_WALKING = 5,
	RACOON_WALKING = 7,

	SMALL_JUMPING = 8,
	BIG_JUMPING = 9,
	RACOON_JUMPING = 11,

	SMALL_RUNNING = 12,
	BIG_RUNNING = 13,
	RACOON_RUNNING = 15,
	SMALL_RUNNING_MAX = 16,
	BIG_RUNNING_MAX = 17,
	RACOON_RUNNING_MAX = 19,

	SMALL_JUMPING_MAX = 20,
	BIG_JUMPING_MAX = 21,
	RACOON_JUMPING_MAX = 23,

	SMALL_BRAKING = 24,
	BIG_BRAKING = 25,
	RACOON_BRAKING = 27,

	RACOON_DESCENDING = 45,
	RACOON_FLYING = 46,

	BIG_JUMPED = 62,

	DIE = 8,

	BIG_DUCKING = 40,
	RACOON_DUCKING = 42
};

enum MarioAction
{
	JUMPING = 0,
	SUPER_JUMPING = 1,
	DONE_JUMPING = 2,
	GAINING_POWER = 3,
	FLYING = 4,
	PEAKING = 5,
	DESCENDING = 6,
	DUCKING = 7,
};

enum MarioState
{
	MARIO_DEAD = 0,
	MARIO_SMALL = 1,
	MARIO_BIG = 2,
	MARIO_RACOON = 3,
};


class Mario : public GameObject
{
	int level;
	int untouchable;
	ULONGLONG untouchable_start;
	ULONGLONG powerTimer, flyTimer;
	ULONGLONG lastTimeGainPower;
	ULONGLONG lastTimeDecreasePowerIdle, lastTimeDecreasePowerMaxHeight;


	float start_x;
	float start_y;

	int powerMeter;

	float last_y;

	bool collision[4] = { 0 };
	bool edge[4] = { 0 };
	bool movement[4] = { 0 };
	bool action[8] = { 0 };

public:
	Mario(float x = 0.0f, float y = 0.0f);
	virtual void Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* colliable_objects = NULL);
	virtual void Render();

	void SetCollision(int direction) { collision[direction] = true; };
	void UnsetCollision(int direction) { collision[direction] = false; };
	bool GetCollision(int direction) { return collision[direction]; }

	void Movement();
	void SetMovement(int direction) { movement[direction] = true; };
	void UnsetMovement(int direction) { movement[direction] = false; };
	bool GetMovement(int direction) { return movement[direction]; }

	void SetAction(int action) { this->action[action] = true; }
	void UnsetAction(int action) { this->action[action] = false; }
	bool GetAction(int action) { return this->action[action]; }

	void SetPowerMeter(int value) { powerMeter = value; }
	int GetPowerMeter() { return powerMeter; }
	void ManagePowerDuration();

	void SetState(int state);
	void StartUntouchable() { untouchable = 1; untouchable_start = GetTickCount64(); }
	void Reset();
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
};