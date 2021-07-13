#include "Mushroom.h"
#include "Game.h"
#include "ScenePlayer.h"
#include "Point.h"
#include "Mario.h"
#include "Debug.h"
#include "Platform.h"
#include "Goomba.h"

Mushroom::Mushroom(float x, float y, int type)
{
	this->x = x;
	this->y = y;
	entryX = x;
	entryY = y;
	SetState(MUSHROOM_GROWING);
	sprite = Sprites::GetInstance()->Get(type);
}

void Mushroom::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;
	right = x + TILE_WIDTH;
	bottom = y + TILE_HEIGHT;
}

void Mushroom::Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects)
{
	GameObject::Update(dt, coObjects);

	std::vector<LPCOLLISIONEVENT> coEvents;
	std::vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	CalculatePotentialCollisions(coObjects, coEvents);
	if (coEvents.size() == 0)
	{
		x += dx;
		y += dy;
	}
	else
	{
		float min_tx, min_ty, nx = 0, ny;
		float rdx = 0;
		float rdy = 0;

		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);
		//DebugOut(L"min_tx %f min_ty %f nx %f ny %f rdx %f rdy %f\n", min_tx, min_ty, nx, ny, rdx, rdy);

		y += min_ty * dy + ny * PUSH_BACK;
		x += min_tx * dx + nx * PUSH_BACK;

		if (ny != 0) vy = 0;

		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];
			if (e->nx != 0 && ny == 0)
			{
				if (dynamic_cast<Platform*>(e->obj))
				{
					this->nx = -this->nx;
				}
				else
				{
					x -= min_tx * dx + nx * PUSH_BACK;
					x += dx;
				}
			}
		}

	}

	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];

	if (state == MUSHROOM_GROWING && entryY - y > MUSHROOM_GROWTH_HEIGHT)
	{
		y = entryY - MUSHROOM_GROWTH_HEIGHT;
		vy = 0;
		SetState(MUSHROOM_MOVING);
	}

	if (state == MUSHROOM_MOVING)
	{
		vx = nx * MUSHROOM_MOVING_SPEED;
		vy += 0.001f * dt;
	}


	//// This is when the mushroom has fully grown
	//if (vy == 0 && vx == 0 && x != entryX)
	//{
	//	float marioX, marioY;
	//	LPSCENE scene = Game::GetInstance()->GetCurrentScene();
	//	((ScenePlayer*)scene)->GetPlayer()->GetPosition(marioX, marioY);

	//	if (entryX >= marioX) nx = 1;
	//	else nx = -1;
	//}

	//DebugOut(L"x %f y %f nx %d ax %f vx %f ay %f vy %f \n", x, y, nx, ax, vx, ay, vy);
}

void Mushroom::Render()
{
	sprite->Draw(nx, x, y, OPAQUED);
	RenderBoundingBox();
}

void Mushroom::SetState(int state)
{
	GameObject::SetState(state);

	switch (state)
	{
	case MUSHROOM_GROWING:
	{
		vy = -MUSHROOM_GROWTH_SPEED;
		float marioX, marioY;
		LPSCENE scene = Game::GetInstance()->GetCurrentScene();
		((ScenePlayer*)scene)->GetPlayer()->GetPosition(marioX, marioY);

		if (entryX > marioX) nx = 1;
		else nx = -1;
		break;
	}
	case MUSHROOM_MOVING:
		vx = nx * MUSHROOM_MOVING_SPEED;
		break;
	}
}
