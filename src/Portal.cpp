#include "Portal.h"

Portal::Portal(
	int width, int height, int scene_id, 
	float destX, float destY, int direction)
{
	this->scene_id = scene_id;
	this->width = width;
	this->height = height;
	this->destX = destX;
	this->destY = destY;
	this->direction = direction;
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

void Portal::GetDestination(float& destX, float& destY)
{
	destX = this->destX;
	destY = this->destY;
}