#include "Platform.h"

void Platform::Render()
{
	RenderBoundingBox();
}

void Platform::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x + TILE_WIDTH;
	b = y + TILE_WIDTH;
}