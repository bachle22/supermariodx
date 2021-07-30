#pragma once
#include "GameObject.h"

constexpr int PLAYER_STATE_IDLE = 0;
constexpr int PLAYER_STATE_RIGHT = 1;
constexpr int PLAYER_STATE_LEFT = 2;
constexpr int PLAYER_STATE_UP = 3;
constexpr int PLAYER_STATE_DOWN = 4;

constexpr float PLAYER_SPEED = .1f;

class MenuPlayer : public GameObject
{
	int sceneId;
	bool movement[4] = { 0 };
	float lastX, lastY;
public:
	MenuPlayer();
	virtual void Render();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	virtual void Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects);
	virtual void SetState(int state);
	virtual void SetPosition(float x, float y);

	int GetSceneId() { return sceneId; };
	void SetMovement(bool* movement);
	bool GetMovement(int direction) { return movement[direction]; }
	void UnsetMovement();
	void SwitchScene();
};
