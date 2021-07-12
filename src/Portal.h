#pragma once

#pragma once

#include "GameObject.h"

/*
	Object that triggers scene switching
*/
class Portal : public GameObject
{
	int scene_id;	// target scene to switch to 

	float width;
	float height;

	virtual void Render();
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);

public:
	Portal(float l, float t, float r, float b, int scene_id);
	int GetSceneId() { return scene_id; }
};