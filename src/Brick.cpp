#include "Brick.h"
#include "Game.h"
#include "Debug.h"

Brick::Brick(float x, float y, int type)
{
	this->type = type;
	isHit = false;
	timer = 0;
	this->x = x;
	this->y = y;
	temp_y = y;
}

void Brick::Render()
{
	animation_set->at(0)->Render(NOFLIP, x, y);
	//RenderBoundingBox();
}

void Brick::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x + TILE_WIDTH;
	b = y + TILE_WIDTH;
}

void Brick::Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects)
{
	GameObject::Update(dt, coObjects);

	if (vy != 0) vy += 0.002f * dt;

	y += dy;

	if (isHit && timer < 100)
	{
		timer += dt;
		vy -= 0.05f;
	}
	else
	{
		timer = 0;
		if (y >= temp_y) vy = 0;
		isHit = false;
	}

	DebugOut(L"vy %d\n", vy);
}

void Brick::Hit()
{
	isHit = true;
}