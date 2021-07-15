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
	this->type = type;

	sprite = Sprites::GetInstance()->Get(type);
	growingDelay = 0;
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

	if (growingDelay < MUSHROOM_GROWING_DELAY)
	{
		growingDelay += dt;
		if (growingDelay >= MUSHROOM_GROWING_DELAY) SetState(MUSHROOM_GROWING);
	}


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

		y += min_ty * dy + ny * PUSH_BACK * 2;
		x += min_tx * dx + nx * PUSH_BACK * 2;

		float entry_vx = vx;
		float entry_vy = vy;

		if (nx != 0) vx = 0;
		if (ny != 0) vy = 0;

		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];
			{
				if (dynamic_cast<Platform*>(e->obj))
				{
					if (e->nx != 0 && ny == 0)
						this->nx = -this->nx;
				}
				else
				{
					x -= min_tx * dx + nx * PUSH_BACK * 2;
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
		vy += MUSHROOM_GRAVITY * dt;
	}
}

void Mushroom::Render()
{
	int clippingHeight = state == MUSHROOM_GROWING ? (int)(entryY - y) : TILE_HEIGHT;
	sprite->DrawClippedSprite(nx, x, y, OPAQUED, TILE_WIDTH, clippingHeight);
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

void Mushroom::AddPoint()
{
	Point* point;
	switch (type)
	{
	case SUPER_MUSHROOM:
		point = new Point(x, y, POINT_1000);
		break;
	case ONE_UP_MUSHROOM:
		point = new Point(x, y, POINT_1UP);
		break;
	default:
		point = new Point(x, y, POINT_100);
	}

	LPSCENE scene = Game::GetInstance()->GetCurrentScene();
	((ScenePlayer*)scene)->AddObject(point);
}