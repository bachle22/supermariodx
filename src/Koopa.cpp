#include "Koopa.h"
#include "Platform.h"
#include "Brick.h"
#include "Debug.h"
#include "Block.h"

Koopa::Koopa(int type)
{
	this->type = type;
	SetState(KOOPA_STATE_WALKING);
	timer = 0;

	leftBounding = 0;
	rightBounding = 0;
}

void Koopa::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;
	switch (state)
	{
	case KOOPA_STATE_WALKING:
		right = x + KOOPA_WIDTH;
		bottom = y + KOOPA_WALKING_HEIGHT;
		break;
	}
}

void Koopa::Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects)
{
	GameObject::Update(dt, coObjects);
	timer += dt;

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

		x += min_tx * dx + nx * PUSH_BACK;
		y += min_ty * dy + ny * PUSH_BACK;

		//if (nx != 0) vx = 0;
		if (ny != 0) vy = 0;

		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];


			if (dynamic_cast<Platform*>(e->obj) || dynamic_cast<Brick*>(e->obj))
			{
				/*DebugOut(L"min_tx %f min_ty %f nx %f ny %f rdx %f rdy %f ", min_tx, min_ty, nx, ny, rdx, rdy);
				DebugOut(L"%f %f %f \n", leftBounding, x, rightBounding);*/

				if (e->ny == -1)
				{

					if (dynamic_cast<Platform*>(e->obj))
					{
						Platform* p = dynamic_cast<Platform*>(e->obj);
						p->Press();
					}
					else if (dynamic_cast<Brick*>(e->obj))
					{
						Brick* b = dynamic_cast<Brick*>(e->obj);
						b->Press();
					}

					GetPlatformBounding(e->obj);
				}

				if (e->nx != 0 && ny != -1) {
					Reverse();
				}
			}

			if (dynamic_cast<Block*>(e->obj))
			{
				x -= min_tx * dx + nx * PUSH_BACK;
				x += dx;
				if (e->ny == -1) GetPlatformBounding(e->obj);
			}

			else {
				x -= min_tx * dx + nx * PUSH_BACK;
				x += dx;
			}
		}
	}



	vy += GLOBAL_GRAVITY * dt;

	if (type == KOOPA_RED)
	{
		if (x < leftBounding - KOOPA_PLATFORM_OFFSET_LEFT && nx < 0) Reverse();
		if (x + KOOPA_PLATFORM_OFFSET_RIGHT > rightBounding && nx > 0) Reverse();
	}

	if (vy >= GLOBAL_TERMINAL_VELOCITY) vy = GLOBAL_TERMINAL_VELOCITY;
}

void Koopa::Render()
{
	int ani = 0;
	switch (type)
	{
	case KOOPA_RED:
		if (state == KOOPA_STATE_WALKING) ani = KOOPA_RED_WALKING;
		break;
	}
	animation_set->at(ani)->Render(nx, x, y);

	RenderBoundingBox();
}

void Koopa::SetState(int state)
{
	GameObject::SetState(state);
	switch (state)
	{
	case KOOPA_STATE_DIE:
		y += KOOPA_BBOX_HEIGHT - KOOPA_BBOX_HEIGHT_DIE + 1;
		vx = 0;
		vy = 0;
		break;
	case KOOPA_STATE_WALKING:
		nx = -1;
		vx = -KOOPA_WALKING_SPEED;
	}

}

void Koopa::Reverse()
{
	this->nx = -this->nx;
	this->vx = -this->vx;
}

void Koopa::GetPlatformBounding(LPGAMEOBJECT obj)
{
	float l, t, r, b;
	obj->GetBoundingBox(l, t, r, b);

	if (leftBounding == 0) leftBounding = l;
	if (rightBounding == 0) rightBounding = r;
	if (leftBounding > l) leftBounding = l;
	if (rightBounding < r) rightBounding = r;
}