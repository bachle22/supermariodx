#include "Goomba.h"
#include "Game.h"
#include "ScenePlayer.h"
#include "Point.h"

Goomba::Goomba()
{
	SetState(GOOMBA_STATE_WALKING);
	timer = 0;
}

void Goomba::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
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

	//
	// TO-DO: make sure Goomba can interact with the world and to each of them too!
	// 

	x += dx;
	y += dy;

	if (vx < 0 && x < 0) {
		x = 0; vx = -vx;
	}

	if (vx > 0 && x > 290) {
		x = 290; vx = -vx;
	}

	if (state == GOOMBA_STATE_DIE)
	{
		timer += dt;
		if (timer >= GOOMBA_REMOVAL_DELAY) Disable();
	}
}

void Goomba::Render()
{
	int ani = GOOMBA_ANI_WALKING;
	if (state == GOOMBA_STATE_DIE) {
		ani = GOOMBA_ANI_DIE;
	}

	animation_set->at(ani)->Render(nx, x, y);
	RenderBoundingBox();
}

void Goomba::SetState(int state)
{
	GameObject::SetState(state);
	switch (state)
	{
	case GOOMBA_STATE_DIE:
	{
		y += GOOMBA_BBOX_HEIGHT - GOOMBA_BBOX_HEIGHT_DIE + 1;
		vx = 0;
		vy = 0;

		Point* point = new Point(x + 2, y, POINT_100);
		LPSCENE scene = Game::GetInstance()->GetCurrentScene();
		((ScenePlayer*)scene)->AddObject(point);

		break;
	}
	case GOOMBA_STATE_WALKING:
		vx = -GOOMBA_WALKING_SPEED;
	}
}

void Goomba::Hit()
{
	SetState(GOOMBA_STATE_DIE);
}
