#include "Coin.h"

Coin::Coin(float x, float y, int type)
{
	this->x = x;
	this->y = y;
	this->type = type;
	entryY = y;
	animation_set = AnimationSets::GetInstance()->Get(70);
}

void Coin::Render()
{
	animation_set->at(0)->Render(NOFLIP, x, y);
	//RenderBoundingBox();
}

void Coin::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	if (type == COIN_HIDDEN) return;
	l = x;
	t = y;
	r = x + COIN_BBOX_WIDTH;
	b = y + COIN_BBOX_HEIGH;
}

void Coin::Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects)
{
	GameObject::Update(dt, coObjects);

	if (vy != 0) vy += 0.001f * dt;
	y += dy;


	if (isThrowing && vy == 0)
	{
		vy = -.35f;
	}

	if (y > entryY)
	{
		vy = 0;
		y = entryY - 20;
		isThrowing = false;
	}
}

void Coin::Throw()
{
	isThrowing = true;
}