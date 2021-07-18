#include "Goomba.h"
#include "Game.h"
#include "ScenePlayer.h"
#include "Point.h"
#include "Platform.h"
#include "Block.h"
#include "Brick.h"

Goomba::Goomba()
{
	SetState(GOOMBA_STATE_WALKING);
	timer = 0;
	
}

void Goomba::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (state == GOOMBA_STATE_HIT) return;
	left = x;
	top = y;
	right = x + GOOMBA_BBOX_WIDTH;

	if (state == GOOMBA_STATE_DIE)
		bottom = y + GOOMBA_BBOX_HEIGHT_DIE;
	else
		bottom = y + GOOMBA_BBOX_HEIGHT;
}

void Goomba::Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects)
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

		float entry_vx = vx;
		float entry_vy = vy;

		if (ny != 0) vy = 0;

		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];
			{
				if (dynamic_cast<Platform*>(e->obj) || dynamic_cast<Brick*>(e->obj))
				{
					if (e->nx != 0 && ny == 0)
						this->nx = -this->nx;
				}
				else {
					x -= min_tx * dx + nx * PUSH_BACK;
					x += dx;
				}
			}
		}
	}

	vy += GLOBAL_GRAVITY * dt;

	if (vx < 0 && x < 0) {
		x = 0; vx = -vx;
	}

	if (vx > 0 && x > 290) {
		x = 290; vx = -vx;
	}
	
	if (vy >= GLOBAL_TERMINAL_VELOCITY) vy = GLOBAL_TERMINAL_VELOCITY;

	if (state == GOOMBA_STATE_DIE)
	{
		timer += dt;
		if (timer >= GOOMBA_REMOVAL_DELAY) Disable();
	}
}

void Goomba::Render()
{
	int ani;
	switch (state)
	{
	case GOOMBA_STATE_DIE:
		ani = GOOMBA_ANI_DIE;
		break;
	default:
		ani = GOOMBA_ANI_WALKING;
		break;
	}
	if (state == GOOMBA_STATE_HIT) animation_set->at(ani)->RenderFirstFrame(x, y, ROTATE180);
	else animation_set->at(ani)->Render(nx, x, y);
	RenderBoundingBox();
}

void Goomba::SetState(int state)
{
	GameObject::SetState(state);
	switch (state)
	{
	case GOOMBA_STATE_WALKING:
		vx = -GOOMBA_WALKING_SPEED;
		break;
	case GOOMBA_STATE_DIE:
	{
		y += GOOMBA_BBOX_HEIGHT - GOOMBA_BBOX_HEIGHT_DIE - 2;
		vx = 0;
		vy = 0;

		Point* point = new Point(x + 2, y, POINT_100);
		LPSCENE scene = Game::GetInstance()->GetCurrentScene();
		((ScenePlayer*)scene)->AddObject(point);
		break;
	}
	case GOOMBA_STATE_HIT:
		Point* point = new Point(x + 2, y, POINT_100);
		LPSCENE scene = Game::GetInstance()->GetCurrentScene();
		((ScenePlayer*)scene)->AddObject(point);

		vx = nx * 0.03f;
		vy = -0.38f;
		break;
	}
}

bool Goomba::Hit()
{
	if (state != GOOMBA_STATE_HIT)
	{
		SetState(GOOMBA_STATE_HIT);
		return true;
	}
	else return false;
}
