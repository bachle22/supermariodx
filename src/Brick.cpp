#include "Game.h"
#include "Brick.h"
#include "Coin.h"
#include "Debug.h"
#include "ScenePlayer.h"

Brick::Brick(float x, float y, int type)
{
	this->type = type;
	isHit = false;
	timer = 0;
	this->x = x;
	this->y = y;
	entryY = y;
	SetState(BRICK_STATE_DEFAULT);

}

void Brick::Render()
{
	int ani = BRICK_ANI_DEFAULT;
	if (state == BRICK_STATE_EMPTY) ani = BRICK_ANI_EMPTY;

	animation_set->at(ani)->Render(NOFLIP, x, y);
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

	if (vy != 0) vy += BRICK_GRAVITY * dt;
	y += dy;

	if (isHit && vy == 0 && state != BRICK_STATE_EMPTY)
	{
		vy = BRICK_ACCELERATION;
	}

	if (y > entryY)
	{
		vy = 0;
		y = entryY;
		isHit = false;
		SetState(BRICK_STATE_EMPTY);
		// Make empty brick harder to hit
		y -= BRICK_EMPTY_SHIFT;
	}
}

void Brick::Hit()
{
	isHit = true;
	if (state == BRICK_STATE_EMPTY) return;
	if (type == BRICK_COIN)
	{
		coin = new Coin(x + COIN_POSITION_OFFSET, y, COIN_HIDDEN);
		LPSCENE scene = Game::GetInstance()->GetCurrentScene();
		((ScenePlayer*)scene)->AddObject(coin);
		coin->Throw();
	}
}