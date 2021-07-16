#pragma once

#include "GameObject.h"
#include "Tail.h"

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
constexpr float MARIO_MAX_TERMINAL_VELOCITY = .25f;
constexpr float MARIO_JUMPING_INERTIA_AX = .3f;
constexpr float MARIO_DEFAULT_VELOCITY_THRESHOLD = .1f;

constexpr float POWER_ALLOW_GAINING_THRESHOLD = .2f;

constexpr int POWER_MAX_FLY_TIME = 3500;
constexpr int POWER_UP_DURATION_STEP = 300;
constexpr int POWER_DOWN_DURATION_STEP = 400;
constexpr int POWER_PEAKED_DECREASE_TIME = 80;
constexpr int POWER_DIRECTION_UNCHANGED_STEP = 350;
constexpr int POWER_DIRECTION_CHANGED_STEP = 200;

constexpr int MAX_POWER_METER = 7;

constexpr int BIG_BBOX_DUCKING_DIFF = 9;
constexpr int RACOON_BBOX_DUCKING_DIFF = 10;

constexpr int SMALL_TO_BIG_DURATION = 1000;
constexpr int BIG_TO_SMALL_DURATION = 600;

constexpr int MARIO_SMALL_WIDTH = 12;
constexpr int MARIO_SMALL_HEIGHT = 16;
constexpr int MARIO_BIG_WIDTH = 14;
constexpr int MARIO_BIG_HEIGHT = 27;
constexpr int MARIO_RACOON_ATTACK_WIDTH = 14;
constexpr int MARIO_RACOON_HEIGHT = 28;
constexpr int MARIO_DUCKING_HEIGHT = 18;

constexpr float MARIO_SMALL_TRANSLATE_X = 2;
constexpr float MARIO_BIG_TRANSLATE_X = 2;
constexpr float MARIO_RACOON_TRANSLATE_X = 10;
constexpr float MARIO_RACOON_SPINNING_X = 7;

enum MarioAnimation
{
	ANI_SMALL_IDLE = 0,
	ANI_BIG_IDLE = 1,
	ANI_RACOON_IDLE = 3,

	ANI_SMALL_WALKING = 4,
	ANI_BIG_WALKING = 5,
	ANI_RACOON_WALKING = 7,

	ANI_SMALL_JUMPING = 8,
	ANI_BIG_JUMPING = 9,
	ANI_RACOON_JUMPING = 11,

	ANI_SMALL_RUNNING = 12,
	ANI_BIG_RUNNING = 13,
	ANI_RACOON_RUNNING = 15,
	ANI_SMALL_RUNNING_MAX = 16,
	ANI_BIG_RUNNING_MAX = 17,
	ANI_RACOON_RUNNING_MAX = 19,

	ANI_SMALL_JUMPING_MAX = 20,
	ANI_BIG_JUMPING_MAX = 21,
	ANI_RACOON_JUMPING_MAX = 23,

	ANI_SMALL_BRAKING = 24,
	ANI_BIG_BRAKING = 25,
	ANI_RACOON_BRAKING = 27,

	ANI_RACOON_DESCENDING = 45,
	ANI_RACOON_FLYING = 46,

	ANI_SMALL_TO_BIG = 48,
	ANI_BIG_TO_SMALL = 49,

	ANI_BIG_JUMPED = 62,

	ANI_DEAD = 8,

	ANI_BIG_DUCKING = 40,

	ANI_RACOON_DUCKING = 42,
	ANI_RACOON_SPINNING = 44,
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
	SPINNING = 8,
};

enum MarioState
{
	MARIO_DEAD = 0,
	MARIO_SMALL = 1,
	MARIO_BIG = 2,
	MARIO_RACOON = 3,
	MARIO_SMALL_TO_BIG = 4,
	MARIO_BIG_TO_SMALL = 5,
};

enum MarioSprite
{
	MARIO_SPINNING_TAIL = 10076,
	MARIO_SPINNING_FRONT = 10075,
	MARIO_SPINNING_BACK = 10083,
};

enum ShiftingSign
{
	SHIFT = 1,
	UNSHIFT = -1,
};


class Mario : public GameObject
{
	int level;
	int untouchable;
	ULONGLONG untouchable_start;
	ULONGLONG powerTimer, flyTimer;
	ULONGLONG lastTimeGainPower;
	ULONGLONG lastTimeDecreasePowerIdle, lastTimeDecreasePowerMaxHeight;
	ULONGLONG animationTimer;

	int score;

	float start_x;
	float start_y;

	int powerMeter;

	float last_y;

	bool edge[4] = { 0 };
	bool movement[4] = { 0 };
	bool action[9] = { 0 };
	bool isAttacking;

	Tail* tail;

public:
	Mario(float x = 0.0f, float y = 0.0f);
	virtual void Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* colliable_objects = NULL);
	virtual void Render();
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);

	void Movement();
	void SetMovement(int direction) { movement[direction] = true; };
	void UnsetMovement(int direction) { movement[direction] = false; };
	bool GetMovement(int direction) { return movement[direction]; }

	void Action();
	void SetAction(int action) { this->action[action] = true; }
	void UnsetAction(int action) { this->action[action] = false; }
	bool GetAction(int action) { return this->action[action]; }

	void SetPowerMeter(int value) { powerMeter = value; }
	int GetPowerMeter() { return powerMeter; }
	void ManagePowerDuration();
	
	void ShiftPosition(int action, int sign);

	void SetState(int state);
	int GetState() { return state; }
	void StartUntouchable() { untouchable = 1; untouchable_start = GetTickCount64(); }
	void Reset();
};