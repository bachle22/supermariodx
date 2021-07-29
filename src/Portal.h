#pragma once

#pragma once

#include "GameObject.h"

/*
	Object that triggers scene switching
*/
class Portal : public GameObject
{
	int scene_id;	// target scene to switch to 

	int width;
	int height;

	float destX, destY;

	virtual void Render();
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);

public:
	Portal(int width, int height, int scene_id, float destX, float destY);
	int GetSceneID() { return scene_id; }
	void GetDestination(float& destX, float& destY);
};