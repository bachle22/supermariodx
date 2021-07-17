#include "Tail.h"
#include "Game.h"
#include "ScenePlayer.h"
#include "Point.h"
#include "Brick.h"
#include "Goomba.h"
#include "Debug.h"

Tail::Tail()
{
	timer = 0;
	Disable();
	
}

void Tail::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;
	right = x + MARIO_BIG_WIDTH;
	bottom = top + TILE_HEIGHT;
}

void Tail::Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects)
{
	GameObject::Update(dt, coObjects);

	// Add velocity to make collision detection work
	vx = TAIL_VELOCITY * nx;

	std::vector<LPCOLLISIONEVENT> coEvents;
	std::vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	CalculatePotentialCollisions(coObjects, coEvents);
	if (coEvents.size() != 0)
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
					Hit* hit = new Hit(x - nx * TAIL_HIT_OFFSET_X, y);
					LPSCENE scene = Game::GetInstance()->GetCurrentScene();
					((ScenePlayer*)scene)->AddObject(hit);
				}
			}
		}

	}

	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];

}

void Tail::SetPosition(float x, float y)
{
	GameObject::SetPosition(x + nx * TAIL_OFFSET_X, y + TAIL_OFFSET_Y);
}

void Tail::Render()
{
	RenderBoundingBox();
}

