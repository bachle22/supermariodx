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
	isOnBlock = false;
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

		y += min_ty * dy + ny * PUSH_BACK;
		x += min_tx * dx + nx * PUSH_BACK;

		//if (nx != 0) vx = 0;
		if (ny != 0) vy = 0;

		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];


			if (dynamic_cast<Block*>(e->obj))
			{
				if (e->ny == -1)
				if (e->ny == -1)
				{
					isOnBlock = true;

					Block* b = dynamic_cast<Block*>(e->obj);
					leftBounding = b->GetLeftX();
					rightBounding = b->GetRightX();

					rightBounding -= KOOPA_WIDTH;

					if (x < leftBounding && nx < 0 && isOnBlock) Reverse();
					else if (x > rightBounding && nx > 0 && isOnBlock) Reverse();
				}
			}

			if (dynamic_cast<Platform*>(e->obj) ||
				dynamic_cast<Brick*>(e->obj))
			{
				isOnBlock = false;
				if (e->nx != 0 && ny != -1) {
					Reverse();
				}
			}
			else {
				x -= min_tx * dx + nx * PUSH_BACK;
				x += dx;
			}
		}
	}



	vy += GLOBAL_GRAVITY * dt;

	//if (x < leftBounding - 1 && nx < 0) Reverse();
	//else if (x +  - 1 > rightBounding && nx > 0) {
	//	Reverse();
	//	DebugOut(L"right to left \n");
	//}

	if (x < leftBounding && nx < 0 && isOnBlock) Reverse();
	else if (x > rightBounding && nx > 0 && isOnBlock) Reverse();

	//else if (x > 596) Reverse();

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