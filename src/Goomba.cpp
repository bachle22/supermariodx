#include "Goomba.h"
#include "Game.h"
#include "ScenePlayer.h"
#include "Point.h"
#include "Platform.h"
#include "Block.h"
#include "Brick.h"
#include "Debug.h"

Goomba::Goomba(int type)
{
	this->type = type;
	if (type == GOOMBA_BROWN) SetState(GOOMBA_STATE_WALKING);
	else SetState(GOOMBA_STATE_SKIPPING);
	timer = 0;
	skippingCounter = 0;
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

		if (ny != 0) vy = 0;

		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];
			{
				if (dynamic_cast<Platform*>(e->obj) ||
					dynamic_cast<Brick*>(e->obj))
				{
					if (e->nx != 0 && ny == 0)Reverse();
					if (e->ny < 0 && state == GOOMBA_STATE_SKIPPING) Skip();
				}
				else if (dynamic_cast<Goomba*>(e->obj))
				{
					Goomba* g = dynamic_cast<Goomba*>(e->obj);
					g->Reverse();
					this->Reverse();
				}
				else {
					x -= min_tx * dx + nx * PUSH_BACK;
					x += dx;
					y -= min_ty * dy + ny * PUSH_BACK;
					y += dy;
				}
			}
		}
	}

	vy += GOOMBA_GRAVITY * dt;

	if (vy >= GLOBAL_TERMINAL_VELOCITY) vy = GLOBAL_TERMINAL_VELOCITY;

	if (state == GOOMBA_STATE_DIE)
	{
		timer += dt;
		if (timer >= GOOMBA_REMOVAL_DELAY) Disable();
	}

	if (state == GOOMBA_STATE_SKIPPING) timer += dt;
}

void Goomba::Render()
{
	int ani;
	D3DXVECTOR2 translation = D3DXVECTOR2(0, 0);
	switch (state)
	{
	case GOOMBA_STATE_DIE:
		ani = GOOMBA_ANI_DIE;
		break;
	case GOOMBA_STATE_SKIPPING:
		if (vy > 0) ani = PARAGOOMBA_ANI_WALKING;
		else ani = PARAGOOMBA_ANI_SKIPPING;
		translation.x -= PARAGOOMBA_TRANS_X;
		translation.y -= PARAGOOMBA_TRANS_Y;
		break;
	default:
		ani = GOOMBA_ANI_WALKING;
		break;
	}
	if (state == GOOMBA_STATE_HIT) animation_set->at(ani)->RenderFirstFrame(x, y, ROTATE180);
	else animation_set->at(ani)->Render(nx, (x), y, VISIBLE, translation);
	RenderBoundingBox();
}

void Goomba::SetState(int state)
{
	GameObject::SetState(state);
	switch (state)
	{
	case GOOMBA_STATE_DIE:
	{
		y += GOOMBA_BBOX_HEIGHT - GOOMBA_BBOX_HEIGHT_DIE - 0.5f;
		vx = 0;
		vy = 0;
		break;
	}
	case GOOMBA_STATE_WALKING:
		vx = -GOOMBA_WALKING_SPEED;
		break;
	case GOOMBA_STATE_SKIPPING:
		vx = -GOOMBA_WALKING_SPEED;
		break;
	case GOOMBA_STATE_HIT:
		vx = nx * GOOMBA_SMACKED_VX;
		vy = -GOOMBA_SMACKED_VY;
		break;
	}
}

bool Goomba::Hit()
{
	if (state != GOOMBA_STATE_HIT)
	{
		SetState(GOOMBA_STATE_HIT);
		
		Point* point = new Point(x + 2, y, POINT_100);
		LPSCENE scene = Game::GetInstance()->GetCurrentScene();
		((ScenePlayer*)scene)->AddObject(point);
		return true;
	}
	else return false;
}

void Goomba::Reverse()
{
	this->vx = -this->vx;
}

void Goomba::Skip()
{
	if (timer < PARAGOOMBA_JUMP_INTERVAL)
	{
		if (skippingCounter == 0) timer = 0;
		skippingCounter++;
		vy = -PARAGOOMBA_SKIP_VY;
		if (skippingCounter > 3)
		{
			skippingCounter = 0;
			vy = -PARAGOOMBA_JUMP_VY;
		}
	}
	if (timer > PARAGOOMBA_JUMP_DELAY)
	{
		timer = 0;
		skippingCounter = 0;
		float marioX, marioY;
		LPSCENE scene = Game::GetInstance()->GetCurrentScene();
		((ScenePlayer*)scene)->GetPlayer()->GetPosition(marioX, marioY);

		if (marioX > x && vx < 0) Reverse();
		else if (marioX < x && vx > 0) Reverse();
	}
}

void Goomba::Downgrade()
{
	if (state > 0) SetState(--state);
	timer = 0;

	int point = POINT_100;
	if (state == GOOMBA_STATE_DIE && type == GOOMBA_RED) point = POINT_200;
	Point* p = new Point(x + 2, y, point);
	LPSCENE scene = Game::GetInstance()->GetCurrentScene();
	((ScenePlayer*)scene)->AddObject(p);

}