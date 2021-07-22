#include "Game.h"
#include "ScenePlayer.h"
#include "Brick.h"
#include "Coin.h"
#include "Mushroom.h"
#include "Mario.h"
#include "Debug.h"

Brick::Brick(int type)
{
	this->type = type;
	isHit = false;
	SetState(BRICK_STATE_DEFAULT);

	isPressed = false;
	timer = 0;
}

void Brick::Render()
{
	int ani = BRICK_ANI_DEFAULT;
	if (state == BRICK_STATE_EMPTY) ani = BRICK_ANI_EMPTY;

	//Keep the hitbox from moving to make collision detection work better
	animation_set->at(ani)->Render(x, imitateY);

	if (isPressed) RenderBoundingBox();
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

	imitateY += dy;

	if (vy != 0) vy += GLOBAL_GRAVITY * dt;

	if (isHit && vy == 0 && state != BRICK_STATE_EMPTY)
	{
		vy = BRICK_ACCELERATION;
		isHit = false;
		SetState(BRICK_STATE_EMPTY);
	}


	if (imitateY > entryY)
	{
		vy = 0;
		imitateY = entryY;
		isHit = false;
		// Make empty brick harder to hit
		y -= BRICK_EMPTY_SHIFT;
	}


	if (isPressed) timer += dt;
	if (timer > BRICK_UNPRESS_DELAY) Unpress();
}

void Brick::Hit()
{
	if (state == BRICK_STATE_EMPTY) return;

	isHit = true;

	switch (type)
	{
	case BRICK_COIN:
	{
		Coin* coin = new Coin(x + COIN_POSITION_OFFSET_X, y - COIN_POSITION_OFFSET_Y, COIN_SMALL);
		LPSCENE scene = Game::GetInstance()->GetCurrentScene();
		((ScenePlayer*)scene)->AddObject(coin);
		coin->Throw();
		break;
	}
	case BRICK_POWER_UP:
	{
		LPSCENE scene = Game::GetInstance()->GetCurrentScene();
		int marioState = ((ScenePlayer*)scene)->GetPlayer()->GetState();
		Mushroom* mushroom = new Mushroom(x + MUSHROOM_OFFSET_X, y, SUPER_MUSHROOM);
		((ScenePlayer*)scene)->AddObject(mushroom);
		break;
	}
	case BRICK_1UP:
		LPSCENE scene = Game::GetInstance()->GetCurrentScene();
		int marioState = ((ScenePlayer*)scene)->GetPlayer()->GetState();
		Mushroom* mushroom = new Mushroom(x + MUSHROOM_OFFSET_X, y, ONE_UP_MUSHROOM);
		((ScenePlayer*)scene)->AddObject(mushroom);
		break;
	}
}

void Brick::SetPosition(float x, float y)
{
	GameObject::SetPosition(x, y);
	this->x = x;
	this->y = y;
	entryY = y;
	imitateY = y;
}

void Brick::Press()
{
	timer = 0;
	if (!isPressed) {
		y += 0.01f;
		isPressed = true;
	}
}

void Brick::Unpress()
{
	if (isPressed) {
		y -= 0.01f;
		isPressed = false;
		timer = 0;
	}
}