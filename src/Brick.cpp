#include "Game.h"
#include "ScenePlayer.h"
#include "Brick.h"
#include "Coin.h"
#include "Mushroom.h"
#include "Mario.h"
#include "Debug.h"

Brick::Brick(float x, float y, int type)
{
	this->type = type;
	isHit = false;
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

void Brick::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;
	right = x + TILE_WIDTH;
	bottom = y + TILE_WIDTH;
}

void Brick::Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects)
{
	GameObject::Update(dt, coObjects);

	if (vy != 0) vy += BRICK_GRAVITY * dt;
	y += dy;

	if (isHit && vy == 0 && state != BRICK_STATE_EMPTY)
	{
		vy = BRICK_ACCELERATION;
		isHit = false;
		SetState(BRICK_STATE_EMPTY);
	}


	if (y > entryY)
	{
		vy = 0;
		y = entryY;
		isHit = false;
		// Make empty brick harder to hit
		y -= BRICK_EMPTY_SHIFT;
	}
}

void Brick::Hit()
{
	if (state == BRICK_STATE_EMPTY) return;

	isHit = true;
	//SetState(BRICK_STATE_EMPTY);

	switch (type)
	{
	case BRICK_COIN:
	{
		Coin* coin = new Coin(x + COIN_POSITION_OFFSET_X, y - COIN_POSITION_OFFSET_Y, COIN_HIDDEN);
		LPSCENE scene = Game::GetInstance()->GetCurrentScene();
		((ScenePlayer*)scene)->AddObject(coin);
		coin->Throw();
		Stats::GetInstance()->AddCoin();
		break;
	}
	case BRICK_POWER_UP:
	{
		LPSCENE scene = Game::GetInstance()->GetCurrentScene();
		int marioState = ((ScenePlayer*)scene)->GetPlayer()->GetState();
		Mushroom* mushroom = new Mushroom(x, y, SUPER_MUSHROOM);
		((ScenePlayer*)scene)->AddObject(mushroom);
		break;
	}
	case BRICK_1UP:
		LPSCENE scene = Game::GetInstance()->GetCurrentScene();
		int marioState = ((ScenePlayer*)scene)->GetPlayer()->GetState();
		Mushroom* mushroom = new Mushroom(x, y, ONE_UP_MUSHROOM);
		((ScenePlayer*)scene)->AddObject(mushroom);
		break;
		break;
	}
}