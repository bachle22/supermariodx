#include "Brick.h"

void Brick::Render()
{
	animation_set->at(0)->Render(-1, x, y);
	RenderBoundingBox();
}

void Brick::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x + TILE_WIDTH;
	b = y + TILE_WIDTH;
}