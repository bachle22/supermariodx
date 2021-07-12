#include "Portal.h"

Portal::Portal(float l, float t, float r, float b, int scene_id)
{
	this->scene_id = scene_id;
	x = l;
	y = t;
	width = r - l + 1;
	height = b - t + 1;
}

void Portal::Render()
{
	RenderBoundingBox();
}

void Portal::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;
	right = x + width;
	bottom = y + height;
}