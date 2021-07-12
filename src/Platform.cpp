#include "Platform.h"

void Platform::Render()
{
	RenderBoundingBox();
}

void Platform::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;
	right = x + TILE_WIDTH;
	bottom = y + TILE_WIDTH;
}