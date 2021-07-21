#include "Koopa.h"
#include "Platform.h"
#include "Brick.h"
#include "Debug.h"
#include "Block.h"
#include "Game.h"
#include "Goomba.h"

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
	right = x + KOOPA_WIDTH;
	bottom = y + height;
}

void Koopa::Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects)
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

		x += min_tx * dx + nx * PUSH_BACK;
		y += min_ty * dy + ny * PUSH_BACK;

		if (ny != 0) vy = 0;

		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];

			if (dynamic_cast<Platform*>(e->obj) || dynamic_cast<Brick*>(e->obj))
			{
				/*DebugOut(L"min_tx %f min_ty %f nx %f ny %f rdx %f rdy %f ", min_tx, min_ty, nx, ny, rdx, rdy);
				DebugOut(L"%f %f %f \n", leftBounding, x, rightBounding);*/

				if (state == KOOPA_STATE_WALKING)
				{
					if (e->ny == -1 && e->nx == 0 && nx == 0)
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
				}


				if (e->nx != 0)
				{
					float l, t, r, b;
					e->obj->GetBoundingBox(l, t, r, b);

					// Only reverse when the next tile is
					// at least 1px higher than current platform
					if (y + height - 1 > t) Reverse();
				}
			}

			else if (dynamic_cast<Block*>(e->obj))
			{
				x -= min_tx * dx + nx * PUSH_BACK;
				x += dx;
				if (e->ny == -1) GetPlatformBounding(e->obj);
			}

			else {
				x -= min_tx * dx + nx * PUSH_BACK;
				x += dx;
			}

			if (state == KOOPA_STATE_ROLLING)
			{
				if (e->nx != 0)
				{
					if (dynamic_cast<Brick*>(e->obj))
					{
						Brick* b = dynamic_cast<Brick*>(e->obj);
						b->Hit();
					}
					else if (dynamic_cast<Goomba*>(e->obj))
					{
						Goomba* g = dynamic_cast<Goomba*>(e->obj);
						g->Hit();
					}
				}
			}
		}
	}



	vy += GLOBAL_GRAVITY * dt;

	if (type == KOOPA_RED && state != KOOPA_STATE_ROLLING)
	{
		if (x < leftBounding - KOOPA_PLATFORM_OFFSET_LEFT && nx < 0) Reverse();
		if (x + KOOPA_PLATFORM_OFFSET_RIGHT > rightBounding && nx > 0) Reverse();

		// Reset Bouding when changing from a platfrom to another
		// by checking looking for significant change in vertical velocity
		if (abs(vy) > KOOPA_PLATFORM_THRESHOLD)
		{
			leftBounding = 0;
			rightBounding = 0;
		};
	}

	if (state == KOOPA_STATE_HIDING)
	{
		timer += dt;
		if (timer > KOOPA_HIDING_INTERVAL)
		{
			SetState(KOOPA_STATE_REVIVING);
			timer = 0;
		}
	}

	if (state == KOOPA_STATE_REVIVING)
	{
		timer += dt;
		// Update x in direct proportion nx
		// Add 1 to prevent division by 0
		if (timer % (1 + timer / KOOPA_REFRESH_CONSTANT) > 0)
		{
			if (nx < 0)
			{
				x += 1;
				nx = 1;
			}
			else
			{
				x -= 1;
				nx = -1;
			}
		}
		if (timer > KOOPA_REVIVING_INTERVAL)
		{
			SetState(KOOPA_STATE_WALKING);
			timer = 0;
			y -= KOOPA_WALKING_HEIGHT - KOOPA_HIDING_HEIGHT;
		}
	}


	if (vy >= GLOBAL_TERMINAL_VELOCITY) vy = GLOBAL_TERMINAL_VELOCITY;
}

void Koopa::Render()
{
	int ani = 0;
	D3DXVECTOR2 translation = D3DXVECTOR2(0, 0);
	switch (type)
	{
	case KOOPA_RED:
		if (state == KOOPA_STATE_WALKING) ani = KOOPA_RED_WALKING;
		else if (state == KOOPA_STATE_HIDING) ani = KOOPA_RED_HIDING;
		else if (state == KOOPA_STATE_ROLLING) ani = KOOPA_RED_ROLLING;
		else if (state == KOOPA_STATE_REVIVING) ani = KOOPA_RED_REVIVING;
		break;
	}

	if (state == KOOPA_STATE_WALKING) translation.y -= KOOPA_WALKING_TRANS_Y;
	else translation.x -= KOOPA_SHELL_TRANS_X;

	//if (state == KOOPA_STATE_REVIVING) animation_set->at(ani)->RenderFlipped(nx, ceil(x), ceil(y));
	animation_set->at(ani)->Render(nx, ceil(x), ceil(y), VISIBLE, translation);

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
		height = KOOPA_WALKING_HEIGHT;
		break;
	case KOOPA_STATE_HIDING:
		vx = 0;
		height = KOOPA_HIDING_HEIGHT;
		y += KOOPA_WALKING_HEIGHT - KOOPA_HIDING_HEIGHT - 1;
		break;
	case KOOPA_STATE_ROLLING:
		vx = nx * KOOPA_ROLLING_SPEED;
		height = KOOPA_HIDING_HEIGHT;
		break;
	case KOOPA_STATE_REVIVING:
		vx = 0;
		height = KOOPA_HIDING_HEIGHT;
		break;
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

void Koopa::Stomp()
{
}