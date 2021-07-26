#include "Portal.h"

Portal::Portal(int width, int height, int scene_id)
{
	this->scene_id = scene_id;
	this->width = width;
	this->height = height;
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