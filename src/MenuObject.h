#pragma once
#include "GameObject.h"

constexpr int OBJECT_TYPE_MENUOBJECT = 10;

constexpr int OBJECT_TYPE_BUSH = 1;
constexpr int OBJECT_TYPE_HAMMERBRO = 2;
constexpr int OBJECT_TYPE_PANEL = 3;
constexpr int OBJECT_TYPE_STOP = 4;

constexpr float HAMMERBRO_SPEED = 0.015f;
constexpr int HAMMERBRO_RANGE = 12;


class MenuObject : public GameObject
{
	int type;
	int sceneId;
	float entryX{}, entryY{};
	bool movement[4] = {0};

	virtual void Render();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	virtual void Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects);
public:
	MenuObject(int type);
	virtual void SetPosition(float x, float y);

	void SetMovement(bool left, bool right, bool up, bool down);
	void SetSceneId(int id) { sceneId = id; }
	int GetSceneId() { return sceneId; }

	bool* GetMovement() { return movement; }
	int GetType() { return type; }
};

