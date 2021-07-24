#include "Game.h"
#include "ScenePlayer.h"
#include "Coin.h"
#include "Point.h"

Coin::Coin()
{
	this->type = COIN_LARGE;
	isThrowing = false;
}

Coin::Coin(float x, float y, int type)
{
	this->x = x;
	this->y = y;
	this->type = type;
	entryY = y;
	animation_set = AnimationSets::GetInstance()->Get(COIN_ANI_SET_ID);
	isThrowing = false;
}

void Coin::Render()
{
	animation_set->at(type)->Render(NOFLIP, x, y);
	RenderBoundingBox();
}

void Coin::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (type == COIN_SMALL) return;
	left = x;
	top = y;
	right = x + COIN_BBOX_WIDTH;
	bottom = y + COIN_BBOX_HEIGH;
}

void Coin::Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects)
{
	GameObject::Update(dt, coObjects);

	if (vy != 0) vy += GLOBAL_GRAVITY * dt;
	y += dy;


	if (isThrowing && vy == 0)
	{
		vy = COIN_VELOCITY;
	}

	// Apply when coin is falling down
	if (y > entryY - COIN_FALLING_FLOOR && vy > 0)
	{
		vy = 0;
		y = entryY - COIN_FALLING_FLOOR;
		isThrowing = false;
		Disable();

		Point* point = new Point(x - POINT_BRICK_OFFSET, y, POINT_100);
		LPSCENE scene = Game::GetInstance()->GetCurrentScene();
		((ScenePlayer*)scene)->AddObject(point);
	}
}

void Coin::Throw()
{
	isThrowing = true;
	Stats::GetInstance()->AddCoin();
}

void Coin::SetPosition(float x, float y)
{
	x -= 1;
	GameObject::SetPosition(x, y);
}

void Coin::Earn()
{
	Stats::GetInstance()->AddCoin();
	Disable();
}